#include "DocumentsPanel.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DocumentManager.h>

#include <QBoxLayout>

DocumentsPanel::DocumentsPanel()
{
    vBoxLayout_->setMargin(0);
    tree_ = new QTreeWidget();
    vBoxLayout_->addWidget(tree_);
    tree_->setDragDropMode(QAbstractItemView::NoDragDrop);
    tree_->headerItem()->setText(0, "File");
    tree_->headerItem()->setText(1, "Type");
    tree_->headerItem()->setText(2, "Path");
    tree_->setSelectionMode(QAbstractItemView::SingleSelection);

    DocumentManager* docMan = ApplicationCore::GetDocumentManager();
    connect(docMan, &DocumentManager::ActiveDocumentChanged, this, &DocumentsPanel::ActiveDocumentChanged);
    connect(docMan, &DocumentManager::DocumentOpened, this, &DocumentsPanel::DocumentOpened);
    connect(docMan, &DocumentManager::DocumentClosed, this, &DocumentsPanel::DocumentClosed);

    connect(tree_, &QTreeWidget::itemSelectionChanged, [=]() {
        if (tree_->selectedItems().size())
        {
            QTreeWidgetItem* item = tree_->selectedItems()[0];
            ApplicationCore::GetDocumentManager()->SetActiveDocument(item->data(0, Qt::UserRole).value<DocumentBase*>());
        }
    });


}

DocumentsPanel::~DocumentsPanel()
{

}

void DocumentsPanel::ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc)
{
    tree_->blockSignals(true);
    for (int i = 0; i < tree_->topLevelItemCount(); ++i)
        if (QTreeWidgetItem* item = tree_->topLevelItem(i))
        {
            item->setSelected(false);
            if (DocumentBase* doc = item->data(0, Qt::UserRole).value<DocumentBase*>())
            {
                if (doc == newDoc)
                {
                    item->setSelected(true);
                }
            }
        }
    tree_->blockSignals(false);
}

void DocumentsPanel::DocumentClosed(DocumentBase* closingDoc)
{
    for (int i = 0; i < tree_->topLevelItemCount(); ++i)
        if (QTreeWidgetItem* item = tree_->topLevelItem(i))
        {
            item->setSelected(false);
            if (DocumentBase* doc = item->data(0, Qt::UserRole).value<DocumentBase*>())
            {
                if (doc == closingDoc)
                {
                    delete item;
                    break;
                }
            }
        }
}

void DocumentsPanel::DocumentOpened(DocumentBase* doc)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, doc->GetFileName());
    item->setText(1, doc->GetHandler()->DocumentTypeName());
    item->setText(2, doc->GetFilePath());
    QVariant var;
    var.setValue<DocumentBase*>(doc);
    item->setData(0, Qt::UserRole, var);
    tree_->addTopLevelItem(item);

    // Connect so that we know about Save/Save-As
    connect(doc, &DocumentBase::PathChanged, [=](QString newPath) {
        item->setText(0, doc->GetFilePath().isEmpty() ? "<unnamed>" : doc->GetFilePath());
    });

}