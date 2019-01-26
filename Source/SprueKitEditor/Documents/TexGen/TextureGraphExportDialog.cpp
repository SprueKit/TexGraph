#include "TextureGraphExportDialog.h"

#include "../../GlobalAccess.h"
#include "../../Localization/LocalizedWidgets.h"

#include "TextureDocument.h"

#include <EditorLib/DocumentManager.h>
#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/Controls/PathPickerWidget.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <QBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>

namespace SprueEditor
{

    TextureGraphExportDialog::TextureGraphExportDialog(QWidget* owner) :
        QDialog(owner)
    {
        setWindowTitle(Localizer::Translate("Export Textures"));

        QVBoxLayout* masterLayout = new QVBoxLayout(this);

    // Export To
        QString path;
        if (auto setting = Settings::GetInstance()->GetValue("Texture Graph Export/Export To"))
            path = setting->value_.toString();
        masterLayout->addWidget(new LocalizedLabel("Export To:"));
        PathPickerWidget* exportDir = new PathPickerWidget(true, false);
        if (QDir(path).exists())
            exportDir->SetPath(path.toStdString());
        masterLayout->addWidget(exportDir);

    // Naming convention
        masterLayout->addWidget(new LocalizedLabel("Naming Convention"));
        // TODO get it from settings
        QLineEdit* namingConvention = new QLineEdit("%1_%2");
        masterLayout->addWidget(namingConvention);
        {
            QGroupBox* conventionInfo = new QGroupBox();
            QVBoxLayout* groupBoxLayout = new QVBoxLayout(conventionInfo);
            QLabel* info = new LocalizedLabel("Use '%1' to insert the name of the file, ie. 'MyStoneTex'");
            QLabel* info2 = new LocalizedLabel("Use '%3' to insert the type of output, ie. 'albedo'");
            groupBoxLayout->addWidget(info);
            groupBoxLayout->addWidget(info2);
            masterLayout->addWidget(conventionInfo);
        }

    // Export frmat
        masterLayout->addWidget(new LocalizedLabel("Export Format"));
        QComboBox* format = new QComboBox();
        format->addItem("PNG");
        format->addItem("TGA");
        format->addItem("HDR");
        format->addItem("DDS Compressed (DXT1 or DXT5)");
        masterLayout->addWidget(format);
        if (auto setting = Settings::GetInstance()->GetValue("Texture Graph Export/Export Format"))
            format->setCurrentIndex(setting->value_.toInt());
        else
            format->setCurrentIndex(0);

    // Button box
        QDialogButtonBox* buttons = new QDialogButtonBox();
        QPushButton* exportButton = new QPushButton("Export");
        buttons->addButton(exportButton, QDialogButtonBox::ButtonRole::AcceptRole);
        connect(buttons->addButton(QDialogButtonBox::Cancel), &QPushButton::clicked, [=]() {
            close();
        });

        masterLayout->addWidget(buttons);

        connect(exportButton, &QPushButton::clicked, [=]() {
            QString exportPath = exportDir->GetPath().c_str();
            if (exportPath.isEmpty() || !QDir(exportPath).exists())
            {
                QMessageBox::warning(0x0, Localizer::Translate("Unable to export Textures"), Localizer::Translate("An export directory must be specified to export textures."));
                return;
            }

            QString convention = namingConvention->text();

            if (convention.isEmpty())
            {
                QMessageBox::warning(0x0, Localizer::Translate("Unable to export Textures"), Localizer::Translate("A naming convention must be specified."));
                return;
            }

            if (auto docMan = Global_DocumentManager())
            {
                if (auto textureDocument = docMan->GetActiveDoc<TextureDocument>())
                {
                    int count = textureDocument->GetExportWorkCount();
                    QProgressDialog progress("Exporting textures", "Cancel", 0, count);
                    progress.show();
                    for (int i = 0;; ++i)
                    {
                        if (!textureDocument->WriteTextures(exportPath, convention, i, format->currentIndex()))
                            break;
                        if (progress.wasCanceled())
                            break;
                        progress.setValue(i + 1);
                    }
                    progress.close();
                    close();
                    if (auto setting = Settings::GetInstance()->GetValue("Texture Graph Export/Folder to export images into"))
                        setting->value_ = QString(exportDir->GetPath().c_str());
                    if (auto setting = Settings::GetInstance()->GetValue("Texture Graph Export/Export Format"))
                        setting->value_ = format->currentIndex();
                }
                else
                {
                    QMessageBox::warning(0x0, Localizer::Translate("Unable to export Textures"), Localizer::Translate("A Texture Graph document must be opened in order to export textures."));
                    return;
                }
            }
        });
    }

    TextureGraphExportDialog::~TextureGraphExportDialog()
    {


    }

}