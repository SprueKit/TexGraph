#include "DocumentViewStackedWidget.h"

#include "../../ApplicationCore.h"

#include <EditorLib/DocumentManager.h>
#include <EditorLib/DocumentBase.h>

#include <QBoxLayout>
#include <QLabel>


DocumentViewStackedWidget::DocumentViewStackedWidget(QWidget* parent) :
    QStackedWidget(parent)
{
    // Add default display widget when nothing meets criteria
    {
        QWidget* voidWidget = new QWidget();
        QVBoxLayout* sub = new QVBoxLayout(voidWidget);

        //TODO: better message
        QLabel* lbl = new QLabel(tr("Nothing to display"));
        lbl->setWordWrap(true);
        sub->addWidget(lbl, 0, Qt::AlignCenter | Qt::AlignTop);
        addWidget(voidWidget);
    }

    connect(ApplicationCore::GetDocumentManager(), &DocumentManager::ActiveDocumentChanged, this, &DocumentViewStackedWidget::OnDocumentChanged);
    connect(ApplicationCore::GetDocumentManager(), &DocumentManager::DocumentViewChanged, this, &DocumentViewStackedWidget::OnDocumentViewChanged);
    connect(ApplicationCore::GetObjectSelectron(), &Selectron::SelectionChanged, this, &DocumentViewStackedWidget::OnSelectionChanged);
}

DocumentViewStackedWidget::~DocumentViewStackedWidget()
{

}

void DocumentViewStackedWidget::AddWidget(IDocTypeChecker* checker, QWidget* widget)
{
    int ctrlIndex = addWidget(widget);
    checkerTable_[checker] = ctrlIndex;
}

void DocumentViewStackedWidget::OnDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc)
{
    OnDocumentViewChanged(newDoc);
}

void DocumentViewStackedWidget::OnDocumentViewChanged(DocumentBase* doc)
{
    int chosenIndex = 0;
    int bestSpecificity = 0;
    if (doc)
    {
        for (auto checker : checkerTable_)
        {
            if (checker.first->CheckDocument(doc))
            {
                int specificity = checker.first->Specificity();
                if (specificity > bestSpecificity)
                {
                    chosenIndex = checker.second;
                    bestSpecificity = specificity;
                }
            }
        }
    }

    // Only change if we need to
    if (currentIndex() != chosenIndex)
        setCurrentIndex(chosenIndex);
}

void DocumentViewStackedWidget::OnSelectionChanged(void* src, Selectron* sel)
{
    OnDocumentViewChanged(ApplicationCore::GetDocumentManager()->GetActiveDocument());
}