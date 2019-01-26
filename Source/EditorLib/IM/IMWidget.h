#pragma once

#include <EditorLib/editorlib_global.h>

#include <EditorLib/ThirdParty/Nuklear/nuklear_header.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <QComboBox>
#include <QCursor>
#include <QWidget>
#include <QElapsedTimer.h>
#include <QTimer>

/// NuklearWidget uses Nuklear for IM style rendering.
/// Events are somewhat excessively stored.
/// Use the helper functions as appropriate to minimize glu-code.
/// The helpers take care of checking for actual changes (important when bogus changes have real consequence).
///
/// ComboBoxes should use the ComboBox() methods to construct embedded QT QComboBoxes. Embedded widgets will behave better in this scenario.
class EDITORLIB_EXPORT IMWidget : public QWidget
{
    Q_OBJECT;
public:
    /// Callback function is available for quick throw-away UIs.
    /// WARNING: IMWidget handles the topmost nk_begin and nk_end
    typedef std::function<void(IMWidget*, nk_context*, QPainter*)> GUI_CALLBACK;

    /// Simple constructor.
    IMWidget(int autoUpdateRate = 100, QWidget* parent = 0x0);
    /// Construct with a delegate method GUI rendering.
    IMWidget(GUI_CALLBACK guiMethod, int autoUpdateRate = 100, QWidget* parent = 0x0);
    /// Destruct.
    virtual ~IMWidget();

    /// Override to handle GUI construction, default implementation does little.
    /// WARNING: IMWidget handles the topmost nk_begin and nk_end
    virtual void GenerateUI(nk_context*, QPainter*);

    /// Set the automatic repaint frequency, use 0 to disable automatic repaint.
    void SetAutoUpdate(int ms);

    /// Widget size will match Nuklear's used window size. This is the main reason handling top-level nk_begin/nk_end outside of GenerateUI.
    void SetMatchSizes(bool state) { matchSizes_ = true; }

    /// Returns true if the text has changed, 4096 character limit
    bool EditString(const char* name, std::string& text);
    /// Returns true if the text has changed, 32,768 character limit
    bool EditText(const char* name, std::string& text);
    /// Edits an int property style, returns true if changed.
    bool EditInt(const char* name, int& value, int min, int max, int increment);
    /// Edits a float property style with a colored underline, returns true if changed.
    bool EditInt(const char* name, int& value, int min, int max, int increment, nk_color col);
    /// Edits a float property style, returns true if changed.
    bool EditFloat(const char* name, float& value, float min, float max, float increment);
    /// Edits a float property style with a colored underline, returns true if changed.
    bool EditFloat(const char* name, float& value, float min, float max, float increment, nk_color col);
    /// Edits an unsigned byte, returns true if changed.
    bool EditByte(const char* name, unsigned char& value);
    /// Edits a bool, returns true if changed.
    bool EditBool(const char* name, bool& value);
    /// Creates a button, returns true if clicked.
    bool ButtonText(const char* name);
    /// Creats a button, returns true if clicked. Includes a tip.
    bool ButtonText(const char* name, const char* tip);
    /// Creates a button with an icon, returns true if clicked.
    bool ButtonImage(const char* name, QIcon icon);
    /// Creates a button with an icon, returns true if clicked. Includes a tip.
    bool ButtonImage(const char* name, const char* tip, QIcon icon);
    /// Creates a button with a QImage, returns true if clicked.
    bool ButtonImage(const char* name, QImage img);
    /// Creates a button with a QImage, returns true if clicked. Includes a tip.
    bool ButtonImage(const char* name, const char* tip, QImage img);
    /// Creates a complete bitfield control (for 32 bits), space between 8x2 clusters for visual clarity.
    bool EditBitfield(const char* name, unsigned& bits);
    /// Simple text only combo-box, items is expected to be a 0x0/null terminated collection.
    bool ComboBox(const char* name, const char** items, int& selectedIndex);
    /// Simple text only combo-box.
    bool ComboBox(const char* name, const char** items, int itemCount, int& selectedIndex);
    /// Simple icon+text combo-box, expects null termination for items and icons to equal items in length.
    bool ComboBox(const char* name, const char** items, QIcon** icons, int& selectedIndex);
    /// Simple icon+text combo-box with a length specifier for items.
    bool ComboBox(const char* name, const char** items, QIcon** icons, int itemCount, int& selectedIndex);
    /// Consumes the next space in the current Nuklear layout, creating a void.
    void EmptySpace();
    /// Places the given widget into the next layout space. Tracking actual use of embedded QT widgets is the caller's responsibility.
    void QTWidget(QWidget* widget);
    /// Returns a QRect for the area the next widget will use. (nk_widget_bounds which peeks)
    QRect GetNextWidgetArea();
    /// Pushes a tooltip into the next open area.
    void PushTooltip(const char* tip);


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

protected:
    /// Override to construct/load appropriate styles.
    virtual void BuildStyles(nk_context*);

    /// Gets an estimated delta-time.
    float GetDelta();

    /// We want the TAB key.
    virtual bool focusNextPrevChild(bool next) Q_DECL_OVERRIDE;
    /// Process mouse buttons.
    virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE { QWidget::mousePressEvent(event);  setFocus(Qt::FocusReason::MouseFocusReason);  update(); }
    /// Process mouse motion.
    virtual void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    /// Process mouse buttons.
    virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE { QWidget::mouseReleaseEvent(event);  update(); }
    /// Process key down (when Qt will actually give it to us).
    virtual void keyPressEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    /// Process key up (when Qt will actually give it to us).
    virtual void keyReleaseEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    /// Record mouse wheel deltas for scrolling.
    virtual void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

    /// Performs the base painting.
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

    /// Renders the nk_commands to the given QPainter.
    virtual void Render(QPainter*);

    /// Measures text for Nuklear.
    static float nkGetTextWidth(nk_handle handle, float height, const char *text, int len);
    /// Copies text from clipboard into the given editor.
    static void nkPaste(nk_handle, nk_text_edit*);
    /// Copies the given text into the clipboard.
    static void nkCopy(nk_handle, const char*, int len);
    /// Handles Nuklear emitting a hotspot, writes them into the hotspot collection.
    static void nkHotspot(nk_handle, struct nk_hotspot);

    QFont font_;
    ///?? after calculating character sizes is it still necessary to keept he font-metrics?
    QFontMetrics metrics_;
    nk_context* context_ = 0x0;
    nk_user_font* nkFont_ = 0x0;

    /// Manages the ambiguity of Nukear's special keys and QT's keys.
    struct KeyRecord {
        int qKey_;
        char charCode_;
        nk_keys nkKey_;
        bool state_ = false;
        KeyRecord(int qKey, char charCode, nk_keys nkKey) : qKey_(qKey), charCode_(charCode), nkKey_(nkKey) { }
        KeyRecord(int qKey, char charCode, nk_keys nkKey, bool state) : qKey_(qKey), charCode_(charCode), nkKey_(nkKey), state_(state) { }
    };

    /// Hotspots indicate regions of the UI that may force repaints, require cursor changes, or provide tips.
    /// They will evolve to support drag-and-drop
    struct HotSpot {
        QRect rect_;
        Qt::CursorShape cursor_;
        bool repaint_ = false;
        const char* tip_ = 0x0;
    };
    std::vector<HotSpot> hotSpots_;

// Tabbing through widgets handling
    /// Current widget sequence for tab focus
    int tabSequence_ = 0;
    /// Index of last widget with active focus in the tab sequence
    int lastActiveWidgetTabIndex_ = 0;
    /// -1 for backward, 1 for forward, 0 for none
    int tabDir_ = 0;
    void ProcessShiftTab();
    /// Handle tab processing
    void ProcessTab();

    /// TODO: is caching via Pixmap viable?
    /// Cache QIcons, how efficient this is depends on what QT does in the background - I have not investigated.
    std::unordered_map<const char*, QIcon> iconCache_;
    /// Cache QImages, how efficient this is depends on what QT does in the background - I have not investigated.
    std::unordered_map<const char*, QImage> imageCache_;

    /// Constructs a key command from QKeyEvent.
    KeyRecord MakeKeyRecord(QKeyEvent* event, bool isDown);
    /// Pushes a record 'as is' into the pending key commands.
    void PushRecord(const KeyRecord& rec);

    /// Key states management.
    std::vector<KeyRecord> keysDown_;
    /// Temporary for keeping the last delta of the mouse wheel.
    int lastWheelDelta_ = 0;
    /// Timer for measuring update deltas.
    QElapsedTimer timer_;
    /// Timer for automatic repaint (for response to foreign changes in values).
    QTimer repaintTimer_;
    /// Callback function for simple UIs.
    GUI_CALLBACK guiCallback_;

    /// If set then the widget will be resized to match the underlying size of the Nuklear window (use when inside of a scroll-area)
    bool matchSizes_ = false;
    /// If not set then we'll call BuildStyles before painting to generate UI styles.
    bool stylesInitialized_ = false;
    /// Stores the computed character widths. TODO: UTF-8/i18n
    int precomputedCharacterWidths_[256];

    struct ComboBoxRecord {
        QComboBox* combo_;
        bool used_ = false;
        int index_ = 0;
        bool justChanged_ = false;
    };
    /// Cache of QComboBoxes, these are instantiated via the "ComboBox()" method and then embedded via QTWidget().
    std::unordered_map<const char*, ComboBoxRecord*> comboBoxCache_;

    ComboBoxRecord* GetComboBoxRecord(const char* name);
    void PrepareComboCache();
    void CleanComboCache();
    void FlushComboCache();
};