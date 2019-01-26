#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/Selectron.h>
#include <EditorLib/Commands/SmartUndoStack.h>

#include <QObject>
#include <QFileInfo>

#include <string>
#include <memory>

class DocumentBase;
class Task;

namespace SprueEditor
{
    class ViewBase;
}


class EDITORLIB_EXPORT DocumentHandler {
public:
    /// Invoked once during start. Suitable place to register document specific commands.
    virtual void OnProgramInitialized() { }
    /// If returning false then the option to create files will not be present.
    virtual bool CanCreateNew() const { return true; }
    /// Implementation should instantiate a new document.
    virtual DocumentBase* CreateNewDocument() = 0;
    /// Implementation should return null if it turns out it cannot open the file. (ie: .xml for everything)
    virtual DocumentBase* OpenDocument(const QString& path) = 0;
    /// Returns format string to use, exmaple: "Particle Effect XML (*xml)"
    virtual QString GetOpenFileFilter() const = 0;
    /// Returns format string to use, example: "Particle Effect XML (*.xml)"
    virtual QString GetSaveFileFilter() const = 0;
    /// Pretty name to use for this type of document.
    virtual QString DocumentTypeName() const = 0;
};

/// Abstracts the handling (save/load/dirty) of a document type
/// Registered documents appear as optional "New" files, intent is to facilitate
//  both multi-function software and to easily create spin off programs (ie. limited to 1 document type)
class EDITORLIB_EXPORT DocumentBase : public QObject
{
    friend class DocumentManager;
    Q_OBJECT;
public:
    /// Construct.
    DocumentBase(DocumentHandler* handler) :
        handler_(handler),
        undoStack_(new SmartUndoStack())
    {
    }

    /// Destruct.
    virtual ~DocumentBase();

    /// Called immediately after this document is activated, but BEFORE the DocumentManager emits the active document changed signal.
    virtual void PreActivate() { }
    /// Called immediately after this document is activated, but AFTER the DocumentManager emits the active document changed signal.
    virtual void PostActivate() { }
    /// Called immediately after this document is deactivated, but BEFORE the DocumentManager emits the active document changed signal.
    virtual void PreDeactivate() { }
    /// Called immediately after this document is deactivated, but AFTER the DocumentManager emits the active document changed signal.
    virtual void PostDeactivate() { }

    /// Saves the document and clears the dirty state.
    virtual bool Save() = 0;
    /// Write into the backup directory a save.
    virtual bool DoBackup(const QString& backupDir) = 0;
    /// Returns true if the document is dirty.
    virtual bool IsDirty() { return dirty_; }
    /// Force the dirty flags to a given state.
    void SetDirty(bool state);

    /// Returns the DocumentHandler responsible for this document type.
    virtual DocumentHandler* GetHandler() { return handler_; }

    /// Returns the full file path.
    virtual QString GetFilePath() const { return filePath_; }

    /// Sets the file path and emits the PathChanged signal.
    virtual void SetFilePath(const QString& path) { filePath_ = path; emit PathChanged(filePath_); }

    /// Returns the short filename (without extension or full path)
    virtual QString GetFileName() const {
        QFileInfo file(GetFilePath());
        if (file.exists())
            return file.baseName();
        return "<unnamed>";
    }

    /// Returns this document's unique undo-stack for history.
    SmartUndoStack* GetUndoStack() const { return undoStack_.get(); }

    /// Get the currently active view for ViewManager processing.
    std::shared_ptr<SprueEditor::ViewBase> GetActiveView() const { return activeView_; }
    /// Returns the index of the current view.
    int GetActiveViewIndex() const;
    /// Sets the active view index.
    virtual void SetActiveView(int index) { emit ViewChanged(this); }

    /// Override to return our widget that should be available in the rendering area's toolshelf.
    virtual QWidget* GetToolshelfWidget() { return 0x0; }

    void AddDependentTask(std::shared_ptr<Task> task);

    virtual bool HasReports() const { return false; }
    virtual void BeginReport() { }
    virtual bool HasExport() const { return false; }
    virtual void BeginExport() { }

    unsigned GetRibbonTabIndex() const { return ribbonTabIndex_; }
    void SetRibbonTabIndex(unsigned idx) { ribbonTabIndex_ = idx; }

    /// Returns the selectron contained.
    Selectron* GetSelectron() { return &documentSelectron_; }
    /// Returns the selectron contained.
    const Selectron* GetSelectron() const { return &documentSelectron_; }

signals:
    void ViewChanged(DocumentBase* doc);
    void DirtyChanged(bool dirty);
    void PathChanged(const QString newPath);

protected:
    /// The selectron containing the document's selection state.
    Selectron documentSelectron_;
    /// This documents Undo/Redo information.
    std::unique_ptr<SmartUndoStack> undoStack_;
    /// Handler that created this document instance/.
    DocumentHandler* handler_ = 0x0;
    /// Path to the file, empty if it is a new document.
    QString filePath_;
    /// View that is active for this document.
    std::shared_ptr<SprueEditor::ViewBase> activeView_;
    /// Contains all of the possible views for the document. Used for Next/Previous view commands.
    std::vector<std::shared_ptr<SprueEditor::ViewBase> > views_;
    /// Tasks that are tied to a specific document need to have Task::Canceled() called so the tasks don't do anything dangerous in Task::FinishTask().
    std::vector<std::weak_ptr<Task> > tasks_;
    /// Dirty indicator.
    bool dirty_ = true;
    /// Index of the last ribbon tab opened.
    unsigned ribbonTabIndex_ = 0;
};