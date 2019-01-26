#include "OpenDocumentsButton.h"

#include "DocumentFauxButton.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/QtHelpers.h>

#include <EditorLib/ThirdParty/QSexyToolTip.h>

#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStyle>


OpenDocumentsButton::OpenDocumentsButton(QWidget* parent) :
    QPushButton(parent)
{
    setText("< none >");
    setObjectName("document_button");

    popupWindow_ = new QSexyToolTip(this);
    QVBoxLayout* layout = new QVBoxLayout(popupWindow_);
    popupWindow_->attach(this);

    documentsWidget_ = new QWidget();
    documentsWidget_->setMinimumWidth(320);
    layout->addWidget(documentsWidget_);
    QVBoxLayout* docLayout = new QVBoxLayout(documentsWidget_);
    docLayout->setMargin(0);
    connect(popupWindow_, &QSexyToolTip::preshow, this, &OpenDocumentsButton::PopupShown);
    connect(ApplicationCore::GetDocumentManager(), &DocumentManager::ActiveDocumentChanged, this, &OpenDocumentsButton::ActiveDocumentChanged);
}

OpenDocumentsButton::~OpenDocumentsButton()
{

}

void OpenDocumentsButton::PopupShown()
{
    QtHelpers::ClearLayout(documentsWidget_->layout());

    documentsWidget_->layout()->blockSignals(true);

    QVBoxLayout* buttonsLayout = (QVBoxLayout*)documentsWidget_->layout();
    DocumentFauxButton* focusButton = 0x0;
    //QPushButton* focusButton = 0x0;
    auto docMan = ApplicationCore::GetDocumentManager();
    for (auto doc : docMan->GetDocuments())
    {
        const bool isActiveDoc = docMan->GetActiveDocument() == doc;
        QHBoxLayout* subLayout = new QHBoxLayout();
        DocumentFauxButton* docButton = 0x0;
        //QPushButton* docButton = 0x0;
        if (doc->IsDirty())
        {
            subLayout->addWidget(docButton = new DocumentFauxButton(doc), 1);
            docButton->setProperty("dirty", true);
            docButton->SetDirty(true);
        }
        else
            subLayout->addWidget(docButton = new DocumentFauxButton(doc), 1);

        if (doc == docMan->GetActiveDocument())
        {
            focusButton = docButton;
            focusButton->SetCurrent(true);
        }

        QPushButton* saveButton = new QPushButton(QIcon(":/Images/godot/icon_save.png"), "");
        subLayout->addWidget(saveButton);

        QPushButton* closeButton = new QPushButton(QIcon(":/Images/godot/icon_close.png"), "");
        subLayout->addWidget(closeButton);

        connect(saveButton, &QPushButton::clicked, [=]() {
            if (docMan->SaveDocument(doc))
            {
                docButton->setProperty("dirty", false);
                docButton->style()->unpolish(docButton);
                docButton->style()->polish(docButton);
                docButton->SetDirty(false);
            }
        });

        connect(closeButton, &QPushButton::clicked, [=]() {
            if (docMan->CloseDocument(doc, true))
            {
                QtHelpers::ClearLayout(subLayout);
                buttonsLayout->removeItem(subLayout);
                delete subLayout;
            }
        });

        connect(docButton, &DocumentFauxButton::Clicked, [=]() {
            if (docMan->GetActiveDocument() != doc)
                docMan->SetActiveDocument(doc);
        });

        connect(docMan, &DocumentManager::ActiveDocumentChanged, docButton, [=](DocumentBase* newDoc, DocumentBase* oldDoc) {
            if (docButton)
            {
                if (docButton->GetDocument() == newDoc || docButton->GetDocument() == oldDoc)
                    docButton->SetCurrent(newDoc == doc);
            }
        });

        buttonsLayout->addLayout(subLayout);
    }

    if (focusButton && !focusButton->hasFocus())
        focusButton->setFocus();

    if (docMan->GetDocuments().empty())
        buttonsLayout->addWidget(new QLabel("< No open documents >"));

    documentsWidget_->layout()->blockSignals(false);
}

void OpenDocumentsButton::ActiveDocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc)
{
    if (oldDoc)
        disconnect(oldDoc, &DocumentBase::PathChanged, this, &OpenDocumentsButton::DocumentPathChanged);
    if (newDoc)
    {
        connect(newDoc, &DocumentBase::PathChanged, this, &OpenDocumentsButton::DocumentPathChanged);
        if (!newDoc->GetFileName().isEmpty())
            setText(newDoc->GetFileName());
        else
            setText("<new - file>");
    }
    else
    {
        setText("< none >");
    }
}

void OpenDocumentsButton::DocumentPathChanged(const QString newPath)
{
    if (auto activeDoc = ApplicationCore::GetDocumentManager()->GetActiveDocument())
        setText(activeDoc->GetFileName());
}