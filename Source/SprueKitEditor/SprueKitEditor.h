#pragma once

#include <EditorLib/DocumentManager.h>
#include <EditorLib/Selectron.h>
#include <EditorLib/Settings/Settings.h>

#include <QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/qstatusbar.h>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QProgressBar;
class QSexyToolTip;
class QStackedWidget;
class Ribbon;

class DocumentBase;
class DocumentManager;
class DocumentsTabBar;
class TaskProcessor;

namespace SprueEditor
{

class GuiBuilder;
class LocalizedDock;

class RenderWidget;
class ViewManager;

/// Root of the user interface.
class SprueKitEditor : public QMainWindow
{
    Q_OBJECT
public:
    /// Construct the frame with the given GUI builder classes.
    SprueKitEditor(Settings* settings, const std::vector<GuiBuilder*>& guiBuilders, const std::vector<DocumentHandler*>& docHandlers, QWidget *parent = 0);
    /// Destruct.
    virtual ~SprueKitEditor();

    /// Returns the singleton for the main GUI.
    static SprueKitEditor* GetInstance();

    /// Prepares the UI for construction and executes all of the "GUI Builder" objects it contains.
    void CreateWidgets();

    /// Returns the internal main window that contains the actual dock widgets.
    QMainWindow* GetDockingWindow() { return internalWindow_; }
    /// Returns the "Selectron" used for managing object selections in the application.
    Selectron* GetObjectSelectron();
    /// Returns the multithreading task processor.
    TaskProcessor* GetTaskProcessor() const { return taskProcessor_; }
    /// Returns the backup multithreading task processor, for lower critical items.
    TaskProcessor* GetSecondaryTaskProcessor() const { return secondaryTaskProcessor_; }
    /// Returns the Widget containing the rendering viewport.
    RenderWidget* GetRenderer() const { return renderWidget_; }
    /// Returns the MS style ribbon control.
    Ribbon* GetRibbon() const { return ribbon_; }
    /// Returns the 3d Viewport management object.
    ViewManager* GetViewManager() const;
    /// Returns the global settings object.
    Settings* GetSettings() const { return settings_; }

    /// Constructs a new menu if it isn't found, otherwise returns the existing one (by "name").
    QMenu* GetOrCreateMenu(const std::string& name, const std::string& title);
    /// Constructs a new toolbar if not found, otherwise returns the existing one (by "name").
    QToolBar* GetOrCreateToolBar(const std::string& name, const std::string& title);
    /// Constructs a new dock widget if not found, otherwise returns the existing one (by "name").
    LocalizedDock* GetOrCreateDock(const std::string& name, const std::string& title, const std::string& defaultAccelerator);
    /// Returns an existing dock sought out by the given "name" when it was constructed.
    LocalizedDock* GetDock(const std::string& name);
    /// Returns the STL container for all contained dock widgets.
    std::map<std::string, LocalizedDock*>& GetAllDocks() { return docks_; }
    /// Returns the STL container for all contained dock widgets.
    const std::map<std::string, LocalizedDock*>& GetAllDocks() const { return docks_; }
    /// Adds a dock that was constructed elsewhere, not advised to use.
    void AddDock(const std::string& name, LocalizedDock* dock);
    /// Returns the STL container for all the toolbars.
    std::map<std::string, QToolBar*> GetAllToolbars() { return toolbars_; }

    /// Save the GUI layout, if string is empty then it will be the default layout
    void SaveLayout(const std::string& name, const std::string& intoFile);
    /// Restore the GUI layout, if string is empty then it will attempt to find the default layout
    void RestoreLayout(const std::string& fromFile);
    /// Get a list of all layouts (and their names)
    std::vector< std::pair<std::string, std::string> > EnumerateLayouts() const;

    /// Returns all of the registered document handlers.
    std::vector<DocumentHandler*>& GetDocumentHandlers() { return documentBuilders_; }
    /// Returns all of the registered document handlers.
    const std::vector<DocumentHandler*>& GetDocumentHandlers() const { return documentBuilders_; }

    template<typename T>
    T* GetDocumentHandler() {
        for (auto object : documentBuilders_)
            if (auto ret = dynamic_cast<T*>(object))
                return ret;
        return 0x0;
    }

signals:
    void WindowMoved();

protected:
    virtual void closeEvent(QCloseEvent*) Q_DECL_OVERRIDE;
    virtual void moveEvent(QMoveEvent*) Q_DECL_OVERRIDE;

private:
    // Create menu bar.
    void CreateMenuBar();
    /// Create the statusbar.
    void CreateStatusBar();

public slots:
    /// Sets the label displayed for the current worker thread task in progress.
    void SetTask(QString lbl);

private slots:
    void HandleBackgroundAction();
    /// Updates task labels.
    void HandleTaskTimeout();
    /// When a document is changed we need to make changes the "view bar" shelf that sits in near the render widget.
    void ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc);
    /// When a document is closed we need to make changes the "view bar" shelf that sits in near the render widget.
    void DocumentClosed(DocumentBase* closingDoc);
    /// Collects and displays search results (assuming anything is found, also responsible for unusual searching [help, etc]).
    void SearchPerformed();

private:
    static SprueKitEditor* instance_;

    /// Menu that will hold the toggle actions for all of the dock widgets.
    QMenu* windowMenu_;
    /// Label that contains the text for displaying information on the current worker thread process.
    QLabel* taskLbl_;
    /// Text editor in the top corner for entering search results.
    QLineEdit* searchText_;
    /// Current display index that the window is living on.
    int monitorIndex_ = -1;

    /// List of menus, by identifier name
    std::map<std::string, QMenu*> menus_;
    /// List of toolbars, by identifier name
    std::map<std::string, QToolBar*> toolbars_;
    /// List of docks, by identifier name
    std::map<std::string, LocalizedDock*> docks_;
    /// Actions, mapped by category name to a list - actions registered here will appear in settings
    std::map<std::string, std::vector<QAction*> > actions_;

    /// This window contains the docks, necessary to avoid toolbar oddities that would occur otherwise.
    QMainWindow* internalWindow_;
    /// Manages the 3d viewport.
    RenderWidget* renderWidget_;
    /// Displays document settings in a bar just above the 3d viewport, used for switching between modes and the like.
    QStackedWidget* viewBar_;
    /// MS style ribbon control as an alternative to menus/toolbars UI, configurable in settings.
    Ribbon* ribbon_;
    /// Tab widget for the current document.
    DocumentsTabBar* documentsTabBar_;
    /// Application menu for the MS style ribbon control's "File" button.
    QSexyToolTip* appMenu_;

    /// Scheduler for long tasks that need to take place in a worker thread.
    TaskProcessor* taskProcessor_;
    TaskProcessor* secondaryTaskProcessor_;
    /// Timer used for updating the status label of the above task processor.
    QTimer* taskTimer_;

    /// Global settings object.
    Settings* settings_;

    /// List of GUI builders that will be invoked during construction
    std::vector<GuiBuilder*> guiBuilders_;
    /// List of document types this program can handle
    std::vector<DocumentHandler*> documentBuilders_;
};

}