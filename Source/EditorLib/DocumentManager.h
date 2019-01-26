#pragma once

#include <EditorLib/editorlib_global.h>

#include <QObject>

class QTimer;

class DocumentBase;
class DocumentHandler;
class Selectron;
class Settings;

/// Manages document behaviour
/// Also places all IO core logic in one place
/// Many controls will connect to signals from this object, thus it should be one of the first things constructed (after settings), 
/// ApplicationCore fulfills this constraint
class EDITORLIB_EXPORT DocumentManager : public QObject
{
    Q_OBJECT
public:
    DocumentManager();
    virtual ~DocumentManager();

    bool SaveDocument(DocumentBase* doc);
    bool SaveActiveDocument();
    bool SaveActiveDocumentAs();
    bool CloseActiveDocument(bool prompt) { return CloseDocument(activeDocument_, prompt); }
    bool CloseDocument(DocumentBase* doc, bool prompt);
    bool CheckCloseProgram();
    void SetActiveDocument(DocumentBase* newDocument);
    DocumentBase* GetActiveDocument() { return activeDocument_; }
    std::vector<DocumentBase*>& GetDocuments() { return documents_; }

    /// Create a new document
    void CreateNewDocument(DocumentHandler*);
    void OpenDocument(DocumentHandler*);

    template<class T>
    T* GetActiveDoc() { return dynamic_cast<T*>(activeDocument_); }

    Selectron* GetSelectron() { return selectron_; }
    Selectron* GetSelectron() const { return selectron_; }

signals:
    /// Emitted whenever the active document is changed (include when changed as a result of closing a document)
    void ActiveDocumentChanged(DocumentBase* newDocument, DocumentBase* oldDocument);
    /// Emitted whenever a document changes the current view mode.
    void DocumentViewChanged(DocumentBase* document);
    /// Emitted whenever a document is opened
    void DocumentOpened(DocumentBase* openedDocument);
    /// Emitted whenever a document is being closed
    void DocumentClosed(DocumentBase* closedDocument);
    /// Emitted before a document is saved, connect to in order to prepare any necessary view data (such as positions)
    void DocumentSaving(DocumentBase* savingDocument);
    /// Document has finished saving, connect to in order to do any special work (probably with the saved file)
    void DocumentSaved(DocumentBase* savedDocument);
    /// Emitted when the state of a document's dirtyness is changed.
    void DocumentDirtyChanged(DocumentBase* document);

private slots:
    void OnDocumentViewChange(DocumentBase* doc);
    void OnAutoSaveTimer();

private:
    void SetupTimer();

    Selectron* selectron_;
    DocumentBase* activeDocument_ = 0x0;
    QTimer* autoSaveTimer_ = 0x0;
    std::vector<DocumentBase*> documents_;
};