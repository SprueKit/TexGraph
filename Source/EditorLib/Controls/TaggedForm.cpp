#include "TaggedForm.h"

#include "PathListWidget.h"
#include "PathPickerWidget.h"

#include <EditorLib/Dialogs/ConfigDlg.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

TaggedForm::TaggedForm(std::vector<TaggedField>& fields, QWidget* parent) :
    QWidget(parent),
    fields_(fields)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    for (auto& fld : fields_)
    {
        if (fld.type_ == TFT_SettingLinked)
        {
            auto pageAndSetting = Settings::GetInstance()->GetPageAndValue(fld.fieldName_.c_str());

            if (pageAndSetting.first && pageAndSetting.second)
            {
                if (QWidget* widget = ConfigDlg::GenerateControls(pageAndSetting.first, pageAndSetting.second))
                {
                    layout->addWidget(new QLabel(pageAndSetting.second->name_));
                    layout->addWidget(widget);
                }
            }
        }
        else if (fld.type_ == TFT_InfoBoxTip)
        {
            QGroupBox* infoBox = new QGroupBox();
            QVBoxLayout* groupBoxLayout = new QVBoxLayout(infoBox);
            groupBoxLayout->addWidget(new QLabel(fld.fieldName_.c_str()));
            layout->addWidget(infoBox);
        }
        else
        {
            layout->addWidget(new QLabel(fld.fieldName_.c_str()));
            switch (fld.type_)
            {
            case TFT_Int: {
                QSpinBox* box = new QSpinBox();
                box->setValue(fld.value_.toInt());
                layout->addWidget(box);
                connect(box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&fld](int val) {
                    fld.value_.setValue<int>(val);
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
            } break;
            case TFT_Float: {
                QDoubleSpinBox* box = new QDoubleSpinBox();
                box->setValue(fld.value_.toDouble());
                layout->addWidget(box);
                connect(box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&fld](double val) {
                    fld.value_.setValue<double>(val);
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
            } break;
            case TFT_String: {
                QLineEdit* edit = new QLineEdit();
                edit->setText(fld.value_.toString());
                layout->addWidget(edit);
                connect(edit, &QLineEdit::textChanged, [&fld](const QString& txt) {
                    fld.value_.setValue<QString>(txt);
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
            } break;
            case TFT_Path: {
                PathPickerWidget* picker = new PathPickerWidget(false, true);
                picker->SetPath(fld.value_.toString().toStdString());
                layout->addWidget(picker);
                connect(picker, &PathPickerWidget::PathChanged, [&fld](const PathPickerWidget* widget, const char* newPath) {
                    fld.value_.setValue<QString>(newPath);
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
                connect(picker, &PathPickerWidget::Deleted, [&fld](const PathPickerWidget* widget) {
                    fld.value_.setValue<QString>("");
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
            } break;
            case TFT_PathList: {
                PathListWidget* widget = new PathListWidget(false);
                widget->SetPaths(fld.value_.toStringList());
                layout->addWidget(widget);
                connect(widget, &PathListWidget::PathsChanged, [&fld, &widget]() {
                    fld.value_.setValue(widget->GetPaths());
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
            } break;
            case TFT_Folder: {
                PathPickerWidget* picker = new PathPickerWidget(true, true);
                picker->SetPath(fld.value_.toString().toStdString());
                layout->addWidget(picker);
                connect(picker, &PathPickerWidget::PathChanged, [&fld](const PathPickerWidget* widget, const char* newPath) {
                    fld.value_.setValue<QString>(newPath);
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
                connect(picker, &PathPickerWidget::Deleted, [&fld](const PathPickerWidget* widget) {
                    fld.value_.setValue<QString>("");
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
            } break;
            case TFT_FolderList: {
                PathListWidget* widget = new PathListWidget(true);
                widget->SetPaths(fld.value_.toStringList());
                layout->addWidget(widget);
                connect(widget, &PathListWidget::PathsChanged, [&fld, &widget]() {
                    fld.value_.setValue(widget->GetPaths());
                    if (fld.changed_)
                        fld.changed_(fld.value_);
                });
            } break;
            }
        }
    }
}

TaggedForm::~TaggedForm()
{

}