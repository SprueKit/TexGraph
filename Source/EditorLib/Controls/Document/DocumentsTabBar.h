#pragma once

#include <EditorLib/editorlib_global.h>

#include <QTabBar>

class DocumentBase;

/// Tab-based document switcher. Shows the active documents and can switch between them.
/// Ribbon mode is preferred for having more screen real-estate.
/// If the ribbon is disabled then this tab bar will be visible, consuming valuable screen real-estate.
class EDITORLIB_EXPORT DocumentsTabBar : public QTabBar
{
    Q_OBJECT;
public:
    /// Construct.
    DocumentsTabBar();
    /// Destruct.
    virtual ~DocumentsTabBar();

public slots:
    /// Adds a tab when a document is opened.
    void DocumentOpened(DocumentBase*);
    /// Removes a tab when a document is closed.
    void DocumentClosed(DocumentBase*);
    /// Changes the tab labeling as the document changes dirty state.
    void DocumentDirtyChanged(DocumentBase*);
    /// Updates the current tab in response to changes in the active document.
    void ActiveDocumentChanged(DocumentBase*, DocumentBase*);
    /// When the current tab is changed, the active document will be changed.
    void IndexChanged(int);
    /// When a tab is closed, attempt to close the document (save/ignore/cancel if dirty).
    void TabCloseRequest(int);
};