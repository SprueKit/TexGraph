#include "LaunchDialog.h"

#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QStackedLayout>

namespace SprueEditor
{

    LaunchDialog::LaunchDialog() :
        QDialog(0, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
    {
        setWindowTitle(Localizer::Translate("Welcome to SprueKit!"));
        this->setModal(true);

        setMinimumSize(640, 480);

        QVBoxLayout* masterLayout = new QVBoxLayout(this);
        QHBoxLayout* layout = new QHBoxLayout();
        masterLayout->addLayout(layout);

        QListWidget* pageList = new QListWidget();
        pageList->setMaximumWidth(200);
        layout->addWidget(pageList, 0);

        pageList->addItem(Localizer::Translate("Quick Start"));
        pageList->addItem(Localizer::Translate("News and Updates"));
        pageList->addItem(Localizer::Translate("Tutorial"));
        pageList->addItem(Localizer::Translate("Manual"));
        pageList->addItem(Localizer::Translate("Licensing"));

        QDialogButtonBox* buttons = new QDialogButtonBox();
        buttons->addButton(QDialogButtonBox::Ok);

        connect(buttons, &QDialogButtonBox::accepted, [=]() {
            this->close();
        });

        connect(buttons, &QDialogButtonBox::rejected, [=]() {
            this->close();
        });

        QCheckBox* showBox = new QCheckBox(Localizer::Translate("Show this dialog on startup?"));
        showBox->setChecked(Settings::GetInstance()->GetValue("General/Show Quick Start Window?")->value_.toBool());
        connect(showBox, &QCheckBox::toggled, [=](bool state) {
            Settings::GetInstance()->SetValue("General/Show Quick Start Window?", state);
        });

        QHBoxLayout* lowerLayout = new QHBoxLayout();
        lowerLayout->addWidget(showBox);
        lowerLayout->addWidget(buttons);
        masterLayout->addLayout(lowerLayout);

        QStackedLayout* stack = new QStackedLayout();
        layout->addLayout(stack, 1);
    }

    LaunchDialog::~LaunchDialog()
    {

    }

}