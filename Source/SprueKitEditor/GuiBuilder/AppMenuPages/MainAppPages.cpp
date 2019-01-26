#include "MainAppPages.h"

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/ThirdParty/QSexyToolTip.h>

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace SprueEditor
{

    NewAppPage::NewAppPage()
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignTop);
        auto editor = SprueKitEditor::GetInstance();
        auto handlers = editor->GetDocumentHandlers();

        QLabel* label = new QLabel("New File");
        label->setObjectName("header");
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        for (auto handler : handlers)
        {
            QPushButton* button = new QPushButton(handler->DocumentTypeName());
            connect(button, &QPushButton::clicked, [=](bool) {
                Global_DocumentManager()->CreateNewDocument(handler);
                if (QSexyToolTip* toolTip = dynamic_cast<QSexyToolTip*>(this->window()))
                    toolTip->hide();
            });
            layout->addWidget(button);
        }
    }

    OpenAppPage::OpenAppPage()
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignTop);
        auto editor = SprueKitEditor::GetInstance();
        auto handlers = editor->GetDocumentHandlers();

        QLabel* label = new QLabel("Open File");
        label->setObjectName("header");
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        for (auto handler : handlers)
        {
            QPushButton* button = new QPushButton(handler->DocumentTypeName());
            connect(button, &QPushButton::clicked, [=](bool) {
                Global_DocumentManager()->OpenDocument(handler);
                if (QSexyToolTip* toolTip = dynamic_cast<QSexyToolTip*>(this->window()))
                    toolTip->hide();
            });
            layout->addWidget(button);
        }
    }

}