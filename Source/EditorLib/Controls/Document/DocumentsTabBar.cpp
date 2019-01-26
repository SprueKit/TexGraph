#include "DocumentsTabBar.h"

#include <EditorLib/DocumentManager.h>
#include <EditorLib/DocumentBase.h>
#include <EditorLib/Styling.h>
#include <EditorLib/ApplicationCore.h>

#include <QVariant>

DocumentsTabBar::DocumentsTabBar()
{
    //setExpanding(true);
    setMovable(true);
    setTabsClosable(true);
    setDocumentMode(true);
    setFont(STYLING_DEFAULT_FONT);

    DocumentManager* documents = ApplicationCore::GetDocumentManager();
    connect(documents, &DocumentManager::DocumentOpened, this, &DocumentsTabBar::DocumentOpened);
    connect(documents, &DocumentManager::DocumentClosed, this, &DocumentsTabBar::DocumentClosed);
    connect(documents, &DocumentManager::ActiveDocumentChanged, this, &DocumentsTabBar::ActiveDocumentChanged);
    connect(documents, &DocumentManager::DocumentDirtyChanged, this, &DocumentsTabBar::DocumentDirtyChanged);
    connect(this, &QTabBar::tabCloseRequested, this, &DocumentsTabBar::TabCloseRequest);
    connect(this, &QTabBar::currentChanged, this, &DocumentsTabBar::IndexChanged);
}

DocumentsTabBar::~DocumentsTabBar()
{

}

void DocumentsTabBar::DocumentOpened(DocumentBase* doc)
{
    blockSignals(true);
    int index = addTab(doc->GetFileName());
    QVariant var;
    var.setValue<void*>(doc);
    setTabData(index, var);

    connect(doc, &DocumentBase::PathChanged, [=](QString newPath) {
        for (int i = 0; i < count(); ++i)
        {
            if (doc == tabData(i).value<void*>())
            {
                setTabText(i, newPath);
                break;
            }
        }
    });
    blockSignals(false);
}

void DocumentsTabBar::DocumentClosed(DocumentBase* doc)
{
    for (int i = 0; i < count(); ++i)
    {
        if (doc == tabData(i).value<void*>())
        {
            removeTab(i);
            break;
        }
    }
}

void DocumentsTabBar::ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* old)
{
    blockSignals(true);
    for (int i = 0; i < count(); ++i)
    {
        if (newDoc == tabData(i).value<void*>())
        {
            setCurrentIndex(i);
            break;
        }
    }
    blockSignals(false);
}

void DocumentsTabBar::IndexChanged(int idx)
{
    blockSignals(true);
    DocumentBase* doc = (DocumentBase*)tabData(idx).value<void*>();
    ApplicationCore::GetDocumentManager()->SetActiveDocument(doc);
    blockSignals(false);
}

void DocumentsTabBar::TabCloseRequest(int idx)
{
    blockSignals(true);
    DocumentBase* doc = (DocumentBase*)tabData(idx).value<void*>();
    ApplicationCore::GetDocumentManager()->CloseDocument(doc, true);
    blockSignals(false);
}

void DocumentsTabBar::DocumentDirtyChanged(DocumentBase* document)
{
    for (int i = 0; i < count(); ++i)
    {
        if (document == tabData(i).value<void*>())
        {
            if (document->IsDirty())
                setTabText(i, QString("*%1*").arg(document->GetFileName()));
            else
                setTabText(i, document->GetFileName());
        }
    }
}