#include "PathFixupDialog.h"

#include "../Localization/LocalizedWidgets.h"
#include "../QtHelpers.h"

#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/Controls/PathPickerWidget.h>

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QLabel>

namespace SprueEditor
{

    PathFixupDialog::PathFixupDialog(const std::vector< std::shared_ptr<PathFixupItem> >& items, QWidget* parent) :
        QDialog(parent),
        items_(items)
    {
        setWindowTitle(Localizer::Translate("Fix Broken Paths"));
        setModal(true);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        if (items.size() > 1)
            mainLayout->addWidget(new LocalizedLabel("A field contains a file path that could not be resolved to an actual file:"));
        else
            mainLayout->addWidget(new LocalizedLabel("Several fields contain file paths that could not be resolved to an actual file:"));

        QScrollArea* scroller = new QScrollArea();
        QVBoxLayout* subLayout = new QVBoxLayout(scroller);
        subLayout->setAlignment(Qt::AlignTop);
        mainLayout->addWidget(scroller);

        for (auto item : items_)
        {
            QVBoxLayout* itemLayout = new QVBoxLayout();

            PathPickerWidget* pathPicker = new PathPickerWidget(false, false);
            pathPicker->SetPath(item->GetCurrentPath().toStdString());
            pathPicker->SetFilter(item->GetFileMask().toStdString());
            // Header style
            QLabel* objName = new QLabel(item->GetObjectName());
            itemLayout->addWidget(objName);
            itemLayout->addWidget(new QLabel(QString("%1 - %2").arg(item->GetKeyName(), item->GetFileTypeName())));
            itemLayout->addWidget(pathPicker);

            connect(pathPicker, &PathPickerWidget::PathChanged, [=](const PathPickerWidget* widget, const char* newPath) {
                // update 
                if (item->SetNewPath(newPath))
                {
                    QtHelpers::ClearLayout(itemLayout);
                    subLayout->removeItem(itemLayout);
                }
            });

            subLayout->addLayout(itemLayout);
        }

        QDialogButtonBox* buttonBox = new QDialogButtonBox();
        connect(buttonBox->addButton(QDialogButtonBox::StandardButton::Close), &QPushButton::clicked, [=]() {
            this->close();
        });
        mainLayout->addWidget(buttonBox);
    }

    PathFixupDialog::~PathFixupDialog()
    {

    }

}