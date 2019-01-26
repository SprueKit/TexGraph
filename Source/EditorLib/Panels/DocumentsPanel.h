#pragma once

#include <EditorLib/editorlib_global.h>

#include <EditorLib/DocumentBase.h>
#include <EditorLib/Controls/ISignificantControl.h>
#include <EditorLib/ScrollAreaWidget.h>

#include <QTreeWidget>

class DocumentBase;

class EDITORLIB_EXPORT DocumentsPanel : public ScrollAreaWidget, public ISignificantControl
{
    Q_OBJECT;
public:
    DocumentsPanel();
    virtual ~DocumentsPanel();

private slots:
    void ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc);
    void DocumentClosed(DocumentBase*);
    void DocumentOpened(DocumentBase*);

private:
    /// Uses QTreeWidget for the list in order to have 2 columns
    QTreeWidget* tree_;
};

Q_DECLARE_METATYPE(DocumentBase*);