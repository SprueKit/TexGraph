#pragma once

#include <EditorLib/editorlib_global.h>

#include <QIcon>
#include <QImage>
#include <QTimer>
#include <QWidget>
#include <QElapsedTimer>

#include <EditorLib/ThirdParty/imgui.h>

#include <functional>
#include <unordered_map>

class QImGuiSlave;
class QSexyToolTip;

/// QT Widget containing a DearIMGUI instance.
/// CRITICAL WARNING: do not launch modal dialogs from directly inside of GenerateUI,
///     use the PushDeferredCall method to queue up a lambda to do any modal tasks at the end of painting
class QImGui : public QWidget
{
    friend class QImGuiSlave;

    Q_OBJECT;
public:
    typedef std::function<void()> DEFERRED_CALL;

    /// Callback function is available for quick throw-away UIs.
    /// WARNING: IMWidget handles the topmost ImGui::Begin() and ImGui::End() for the window.
    /// Return false if you've interrupted painting in some way, true if painting is healthy
    typedef std::function<void(QImGui*, QPainter*, const QSize&)> GUI_CALLBACK;

    /// Construct with a specified update rate.
    QImGui(int autoUpdateFreq = 110 /*9 fps*/);
    /// Construct with a callback function for rendering.
    QImGui(GUI_CALLBACK callback, int autoUpdateFreq = 100);
    /// Destruct.
    virtual ~QImGui();

    /// Set the automatic repaint frequency, use 0 to disable automatic repaint.
    void SetAutoUpdate(int ms);

// Utility functions

    /// Helper for editing a std::string of arbitrary length limitations.
    template<size_t LEN = 1024>
    bool EditString(const char* label, std::string& str)
    {
        static char buffer[LEN];
        memset(buffer, 0, LEN);
        memcpy(buffer, str.c_str(), std::min(str.length(), LEN-1));
        if (ImGui::InputText(label, buffer, LEN))
        {
            str = buffer;
            return true;
        }
        return false;
    }

    /// Helper for editing a QString of arbitrary length limitations.
    template<size_t LEN = 1024>
    bool EditString(const char* label, QString& str)
    {
        static char buffer[LEN];
        memset(buffer, 0, LEN);
        auto data = str.toUtf8();
        memcpy(buffer, data.constData(), std::min(data.length(), LEN-1));
        if (ImGui::InputText(label, buffer, LEN))
        {
            str = buffer;
            return true;
        }
        return false;
    }

    /// Prepares spacing for a # of widgets following the pattern of ImGui's grouped widgets.
    void SetupPropertyStyleSpacing(int count, float width = 0);

    /// Push a function to call at the end of rendering, use this to launch modal dialogs (waiting until the end of paint prevents scroll-bar reseting)
    void PushDeferredCall(DEFERRED_CALL call, bool resetMouse = false) { if (deferredCall_ == nullptr) deferredCall_ = call; deferredResetMouse_ = resetMouse; }

    /// Use to query for the need to do 1-time-only tasks.
    bool IsFirstRun() const { return isFirstRun_; }

    bool IsAutoFit() const { return autoFit_; }
    void SetAutoFit(bool state) { autoFit_ = state; }

    /// Register a texture for use inside of ImGui calls.
    ImTextureID RegisterTexture(QImage* image);
    /// Register a texture for use inside of ImGui calls.
    ImTextureID RegisterTexture(QPixmap* image);

    enum DragDropTraits {
        DDT_DropOn = 1,             // Can drop onto this rect
        DDT_DropAbove = 1 << 1,     // Can drop above this rect
        DDT_DropBelow = 1 << 2,     // Can drop below this rect
        DDT_Drag = 1 << 3,          // Can drag this rect
    };

    /// Creates a drag/drop target for the last item.
    void CreateDragDropTarget(int typeCode, void* data, int traits);
    /// Creates an explicit drag/drop target for a given rect.
    void CreateDragDropTarget(QRect rect, int typeCode, void* data, int traits);

    // We attach to our slaves as an event filter to reroute their commands to us.
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

    // Wrap into a standard QSexyToolTip ... don't forget to set a minimum size for the QImGui instance and attach the flyout to a button.
    static QSexyToolTip* WrapInPopup(QImGui* self, QWidget* owner = 0x0);

public slots:
    // Slot for cutting anything we might be able to, likely invoked by a top-level dispatch through metaObject.
    void cut();
    // Slot for copying anything we might be able to, likely invoked by a top-level dispatch through metaObject.
    void copy();
    // Slot for pasting anything we might be able to, likely invoked by a top-level dispatch through metaObject.
    void paste();
    /// Slot for performing undo, likely invoked by a top-level dispatch through metaObject.
    void undo();
    /// Slot for performing redo, likely invoked by a top-level dispatch through metaObject.
    void redo();

signals:
    void SignalSize(QSize);

protected:
    /// When called your ImGUI context will be fully created, return false if you have distrubed painting (modal dialog was shown)
    virtual void GenerateUI(QPainter* painter, const QSize& size);

    /// Called immediately after initialize so that styles for a window can be prepared.
    virtual void BuildStyles();

    /// When we update we want to restart the timer (if active) to avoid excessive fast repaints.
    virtual void DoUpdate();

private:
    virtual void hideEvent(QHideEvent*) Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    virtual void moveEvent(QMoveEvent*) Q_DECL_OVERRIDE;
    virtual void focusOutEvent(QFocusEvent*) Q_DECL_OVERRIDE;
    virtual bool focusNextPrevChild(bool next) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent*)Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent*)Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;
    virtual void dragLeaveEvent(QDragLeaveEvent*) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QDragMoveEvent*) Q_DECL_OVERRIDE;
    virtual void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

    float GetEstimatedDeltaTime();

    /// Called from constructor to get things ready.
    void Initialize();
    /// Performs IMGUI specific preparations and then calls GenerateUI to render.
    void Render(QPainter* painter);
    /// Takes care of local render preparations (clearing state, first-time run tasks, etc).
    void PrepareRender();
    /// Commits IMGUI calls for rendering.
    void EndRender();
    void Destroy();

    /// Mouse button state.
    bool mouseButtons_[3];
    /// Hack used to force popup loss (if it responds to click)
    bool forceFakeMouse_ = false;
    /// Key tracking
    std::vector<bool> keysDown_;
    /// Text input from keyPressEvent
    QString textInput_;

    /// Timer for measuring update deltas.
    QElapsedTimer timer_;
    /// Timer for automatic repaint (for response to foreign changes in values).
    QTimer repaintTimer_;
    /// Last wheel delta encountered.
    int lastWheelDelta_ = 0;
    /// Force ctrl to active in the next update (cut/copy/etc).
    bool forceCtrl_ = false;
    /// If non-zero we'll remove this keycode.
    int disposeKeycode_ = 0;
    /// So we can restore ourselves
    int assignedUpdateRate_ = 0;
    /// Tracks if this is the first execution of painting.
    bool isFirstRun_ = true;
    /// Will automatically resize to fit contents.
    bool autoFit_ = false;
    QSize autoFitSize_;

    /// The font used for text display.
    QFont font_;
    /// The font metrics used for measurement.
    QFontMetrics metrics_;
    /// Our DearIMGUI context for this window.
    ImGuiContext* imguiContext_ = 0x0;
    /// If present then we will use a (probably massive) lambda statement for UI generation.
    GUI_CALLBACK guiCallback_;
    
// Images
    struct ImageInfo {
        QPixmap* pixmap_ = 0x0;
        QImage* image_ = 0x0;

        ImageInfo(QPixmap* pix) : pixmap_(pix) { }
        ImageInfo(QImage* img) : image_(img) { }
    };
    std::vector<ImageInfo> imageInfos_;

// Drag and Drop
    struct DragDropTarget {
        /// Rectangular picking area for the target.
        QRect rect_;
        int traits_;
        int typeCode_;
        void* data_;

        void Reset() { data_ = 0x0; typeCode_ = INT_MIN, traits_ = INT_MIN; };
    };
    std::vector<DragDropTarget> dragDropTargets_;
    /// Finds a drag/drop-target at the given mouse position with the specified traits. Returns true if one was found.
    bool GetDropTarget(const QPoint& pos, int traits, DragDropTarget* out);

// Slaves and popups
    /// Creates (or reuses) a slave window for display as a popup window.
    QImGuiSlave* GetOrCreateSlave(const char* name);
    /// List of slaves that have recently been acquired.
    std::vector<QImGuiSlave*> recentSlaves_;
    /// Lists of slaves that were used at one point but lost relevance and are now ready for reuse.
    std::vector<QImGuiSlave*> availableSlaves_;

// Render state
    /// The address of this will be given so these can be retrieved during rendering the draw lists.
    std::pair<QPainter*, QImGui*> ourPaintCallbackData_;

// Statics
    /// Global font
    static ImFont* imFont_;
    static ImFont* CreateFont();

    // Dear IMGUI callbacks
    static void RenderDrawData(ImDrawData*);
    static void RenderDrawList(QImGui*, QPainter* painter, ImDrawList* list, const QPoint& paintOffset);
    static const char* ReadClipboard(void* user_data);
    static void WriteClipboard(void* user_data, const char* text);
    static void ShowTooltip(void* user_data, const char* text);

    // If existing this method will be called when painting finishes. Used to perform modal tasks at a safe point in time.
    DEFERRED_CALL deferredCall_;
    bool deferredResetMouse_ = false;
    int paintDepth_ = 0;
};