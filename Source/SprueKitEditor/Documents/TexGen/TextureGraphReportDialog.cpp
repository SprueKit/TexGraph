#include "TextureGraphReportDialog.h"

#include "../../Localization/LocalizedWidgets.h"

#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/Controls/PathPickerWidget.h>

#include <SprueEngine/Reports/TextureGraphReport.h>

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QRadioButton>

namespace SprueEditor
{

    TextureGraphReportDialog::TextureGraphReportDialog() :
        QDialog(0x0)
    {
        setModal(true);
        setWindowTitle(Localizer::Translate("Generate Texture Graph Report"));

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

        mainLayout->addWidget(new LocalizedLabel("Report Title"));
        QLineEdit* titleWidget = new QLineEdit("My Report");
        mainLayout->addWidget(titleWidget);

        QTabWidget* tabs = new QTabWidget();
        mainLayout->addWidget(tabs);

    // Single file report
        QWidget* singleReportWidget = new QWidget();
        tabs->addTab(singleReportWidget, "Single File");
        QVBoxLayout* singleLayout = new QVBoxLayout(singleReportWidget);
        QLabel* fileLabel = new LocalizedLabel("Select File");
        PathPickerWidget* filePicker = new PathPickerWidget(false);
        singleLayout->addWidget(fileLabel);
        singleLayout->addWidget(filePicker);

    // Many reports
        QWidget* manyReportWidget = new QWidget();
        tabs->addTab(manyReportWidget, "Batch Folder");
        QVBoxLayout* manyReportLayout = new QVBoxLayout(manyReportWidget);
        QLabel* folderLabel = new LocalizedLabel("Select Folder");
        PathPickerWidget* folderPicker = new PathPickerWidget(true);
        QCheckBox* subDirsCheck = new LocalizedCheckBox("Include Subdirectories");

        manyReportLayout->addWidget(folderLabel);
        manyReportLayout->addWidget(folderPicker);
        manyReportLayout->addWidget(subDirsCheck);


    // Dialog buttons
        QDialogButtonBox* buttonBox = new QDialogButtonBox();
        QPushButton* generateButton = new LocalizedButton("Generate", "Output the report");
        buttonBox->addButton(generateButton, QDialogButtonBox::ButtonRole::AcceptRole);

        connect(generateButton, &QPushButton::clicked, [=]() {
            QString filePath = QFileDialog::getSaveFileName(0x0, "", QString(), "HTML File (*.html)");
            if (!filePath.isEmpty())
            {
                if (tabs->currentIndex() == 0)
                {
                    SprueEngine::TextureGraphReport report("Test Report", filePath.toStdString(), filePicker->GetPath());
                }
                else
                {
                    QDir dir(folderPicker->GetPath().c_str());
                    std::vector<std::string> files;
                    for (auto entry : dir.entryInfoList())
                    {
                        if (entry.isFile() && entry.completeSuffix().contains("xml") || entry.completeSuffix().contains("gbin"))
                            files.push_back(entry.absoluteFilePath().toStdString());
                        else if (entry.isDir())
                        {
                            //TODO
                        }
                    }
                    SprueEngine::TextureGraphReport report("Test Report", filePath.toStdString(), files);
                }
                close();
            }
        });
        connect(buttonBox->addButton(QDialogButtonBox::StandardButton::Cancel), &QAbstractButton::clicked, [=]() {
            close();
        });
        mainLayout->addWidget(buttonBox);
    }

    TextureGraphReportDialog::~TextureGraphReportDialog()
    {

    }

}