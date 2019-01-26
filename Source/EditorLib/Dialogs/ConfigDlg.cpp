#include "ConfigDlg.h"

#include <EditorLib/ScrollAreaWidget.h>
#include <EditorLib/Localization/Localizer.h>

#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsBitNames.h>
#include <EditorLib/Controls/PathListWidget.h>
#include <EditorLib/Controls/PathPickerWidget.h>
#include <EditorLib/Commands/RegisteredAction.h>
#include <EditorLib/Controls/SingleKeySequenceEdit.h>

#include <QCheckbox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QStackedLayout>
#include <QTabWidget>
#include <QToolBox>
#include <QPushButton>

ConfigDlg* ConfigDlg::instance_ = 0x0;

ConfigDlg::ConfigDlg(Settings* settings) :
    QDialog(0, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
    setWindowTitle(Localizer::Translate("Settings"));
    this->setModal(true);

    setMinimumSize(1000, 640);
    QVBoxLayout* masterLayout = new QVBoxLayout(this);
    QHBoxLayout* layout = new QHBoxLayout();
    masterLayout->addLayout(layout);

    QListWidget* optionList = new QListWidget();
    optionList->setMaximumWidth(200);
    layout->addWidget(optionList, 0);

    QDialogButtonBox* buttons = new QDialogButtonBox();
    buttons->addButton(QDialogButtonBox::Ok);
    //buttons->addButton(QDialogButtonBox::Cancel);

    connect(buttons, &QDialogButtonBox::accepted, [=]() {
        this->close();
    });

    connect(buttons, &QDialogButtonBox::rejected, [=]() {
        this->close();
    });

    QStackedLayout* stack = new QStackedLayout();
    layout->addLayout(stack, 1);

    // List all of the property pages and their properties
    for (auto page : settings->GetPages())
    {
        if (page->IsSecret())
            continue;

        QListWidgetItem* item = new QListWidgetItem(Localizer::Translate(page->GetName()));
        item->setToolTip(Localizer::Translate(page->GetTip()));
        optionList->addItem(item);

        ScrollAreaWidget* widget = new ScrollAreaWidget();
        widget->GetLayout()->setAlignment(Qt::AlignTop);
        QFormLayout* layout = new QFormLayout();
        layout->setAlignment(Qt::AlignTop);

        QLabel* title = new QLabel(Localizer::Translate(page->GetName()));
        title->setToolTip(page->GetTip());
        title->setAlignment(Qt::AlignCenter | Qt::AlignTop);
        widget->GetLayout()->addWidget(title, 0);
        title->setFont(QFont("Arial", 14));

        for (auto setting : *page)
        {
            QLabel* label = new QLabel(Localizer::Translate(setting->name_));
            label->setToolTip(setting->tip_);

            if (QWidget* control = GenerateControls(page, setting))
                layout->addRow(label, control);

            widget->GetLayout()->addLayout(layout);
            stack->addWidget(widget);
        }
    }

    // List all user customizable bit field names
    for (auto bitNames : settings->GetAllBitNames())
    {
        optionList->addItem(Localizer::Translate(bitNames->GetName()));

        ScrollAreaWidget* widget = new ScrollAreaWidget();
        widget->GetLayout()->setAlignment(Qt::AlignTop);
        QFormLayout* layout = new QFormLayout();
        layout->setAlignment(Qt::AlignTop);

        QLabel* title = new QLabel(Localizer::Translate(bitNames->GetName()));
        title->setAlignment(Qt::AlignCenter | Qt::AlignTop);
        title->setFont(QFont("Arial", 14));
        widget->GetLayout()->addWidget(title, 0);

        for (unsigned i = 0; i < bitNames->size(); ++i)
        {
            QLabel* lbl = new QLabel(QString::number(i + 1));
            QLineEdit* lineEdit = new QLineEdit((*bitNames)[i]);
            connect(lineEdit, &QLineEdit::textChanged, [=](const QString& text) {
                (*bitNames)[i] = text;
                settings->NotifyBitNamesChanged(bitNames->GetName());
            });
            connect(settings, &Settings::BitnamesChanged, [=](Settings*) {
                lineEdit->blockSignals(true);
                lineEdit->setText((*bitNames)[i]);
                lineEdit->blockSignals(false);
            });
            layout->addRow(lbl, lineEdit);
        }

        widget->GetLayout()->addLayout(layout);
        stack->addWidget(widget);
    }

    QListWidgetItem* shortCutItem = new QListWidgetItem(Localizer::Translate("Shortcuts"));
    shortCutItem->setToolTip(Localizer::Translate("Configure the accelerator command keys for the application"));
    optionList->addItem(shortCutItem);
    optionList->setItemSelected(optionList->item(0), true);

    // Create shortcuts
    {
        ScrollAreaWidget* widget = new ScrollAreaWidget();
        widget->GetLayout()->setAlignment(Qt::AlignTop);

        QLabel* title = new QLabel(Localizer::Translate("Short Cuts"));
        title->setAlignment(Qt::AlignCenter | Qt::AlignTop);
        title->setFont(QFont("Arial", 14));
        widget->GetLayout()->addWidget(title, 0);

        std::vector<RegisteredAction*> actions = RegisteredAction::GetList();
        std::sort(actions.begin(), actions.end(), [=](RegisteredAction* lhs, RegisteredAction* rhs) {
            return lhs->GetActionClass().compare(rhs->GetActionClass(), Qt::CaseInsensitive) == -1;
        });

        QFormLayout* layout = 0x0;
        QString lastClass;
        QTabWidget* tabs = new QTabWidget();
        widget->GetLayout()->addWidget(tabs, 1);
        //QToolBox* toolbox = new QToolBox();
        //widget->GetLayout()->addWidget(toolbox, 1);

        for (auto item : RegisteredAction::GetList())
        {
            if (layout == 0x0 || lastClass != item->GetActionClass())
            {
                lastClass = item->GetActionClass();
                QWidget* junk = new QWidget();
                layout = new QFormLayout(junk);
                tabs->addTab(junk, item->GetActionClass());
                //toolbox->addItem(junk, item->GetActionClass());
            }

            QLabel* title = new QLabel(Localizer::Translate(item->GetOriginalName()));
            SingleKeySequenceEdit* editor = new SingleKeySequenceEdit(item->GetShortCut());
            layout->addRow(title, editor);
            connect(editor, &QKeySequenceEdit::keySequenceChanged, [=](const QKeySequence& txt) {
                item->SetShortcut(txt);
            });
        }

        stack->addWidget(widget);
    }

    stack->setCurrentIndex(0);

    connect(optionList, &QListWidget::itemSelectionChanged, [=]() {
        stack->setCurrentIndex(optionList->currentRow());
    });

    masterLayout->addWidget(buttons);
}

ConfigDlg::~ConfigDlg()
{

}

void ConfigDlg::Show(Settings* settings)
{
    if (instance_ == 0x0)
        instance_ = new ConfigDlg(settings);
    instance_->show();
}

QWidget* ConfigDlg::GenerateControls(SettingsPage* page, SettingValue* setting)
{
    auto settings = Settings::GetInstance();
    if (setting->type_ == ST_VARIANT_TYPE)
    {
        switch (setting->defaultValue_.type())
        {
        case QVariant::Bool: {
            QCheckBox* checkBox = new QCheckBox("");
            checkBox->setChecked(setting->value_.toBool());
            connect(checkBox, &QCheckBox::toggled, [=](bool value) {
                page->SetValue(setting->name_, value);
                setting->Notify();
            });
            connect(setting, &SettingValue::Changed, [=](const QVariant& value) {
                checkBox->blockSignals(true);
                checkBox->setChecked(setting->value_.toBool());
                checkBox->blockSignals(false);
            });
            return checkBox;
        } break;
        case QVariant::Int: {
            QSpinBox* spinBox = new QSpinBox();
            spinBox->setMinimum(0);
            spinBox->setMaximum(INT_MAX);
            spinBox->setValue(setting->value_.toInt());
            connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
                page->SetValue(setting->name_, value);
                setting->Notify();
            });
            connect(setting, &SettingValue::Changed, [=](const QVariant& value) {
                spinBox->blockSignals(true);
                spinBox->setValue(setting->value_.toInt());
                spinBox->blockSignals(false);
            });
            return spinBox;
        } break;
        case QVariant::Double: {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox();
            spinBox->setMinimum(-10000);
            spinBox->setMaximum(10000);
            spinBox->setValue(setting->value_.toDouble());
            connect(spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double value) {
                page->SetValue(setting->name_, value);
                setting->Notify();
            });
            connect(setting, &SettingValue::Changed, [=](const QVariant&) {
                spinBox->blockSignals(true);
                spinBox->setValue(setting->value_.toDouble());
                spinBox->blockSignals(false);
            });
            return spinBox;
        } break;
        case QVariant::String: {
            QLineEdit* lineEdit = new QLineEdit();
            lineEdit->setText(setting->value_.toString());
            connect(lineEdit, &QLineEdit::textChanged, [=](const QString& newText) {
                page->SetValue(setting->name_, newText);
                setting->Notify();
            });
            connect(setting, &SettingValue::Changed, [=](const QVariant&) {
                lineEdit->blockSignals(true);
                lineEdit->setText(setting->value_.toString());
                lineEdit->blockSignals(false);
            });
            return lineEdit;
        } break;
        case QVariant::KeySequence: {
            SingleKeySequenceEdit* editor = new SingleKeySequenceEdit(setting->value_.value<QKeySequence>());
            connect(editor, &SingleKeySequenceEdit::keySequenceChanged, [=](const QKeySequence& seq) {
                page->SetValue(setting->name_, seq);
                setting->Notify();
            });
            connect(setting, &SettingValue::Changed, [=](const QVariant&) {
                editor->blockSignals(true);
                editor->setKeySequence(setting->value_.value<QKeySequence>());
                editor->blockSignals(false);
            });
            return editor;
        } break;
        }
    }
    else if (setting->type_ == ST_FILE)
    {
        PathPickerWidget* picker = new PathPickerWidget(false);
        picker->SetPath(setting->value_.toString().toStdString());
        connect(picker, &PathPickerWidget::PathChanged, [=](const PathPickerWidget* widget, const char* newPath) {
            page->SetValue(setting->name_, newPath);
            setting->Notify();
        });
        connect(setting, &SettingValue::Changed, [=](const QVariant&) {
            picker->blockSignals(true);
            picker->SetPath(setting->value_.toString().toStdString());
            picker->blockSignals(false);
        });
        return picker;
    }
    else if (setting->type_ == ST_PATH)
    {
        PathPickerWidget* picker = new PathPickerWidget(true);
        picker->SetPath(setting->value_.toString().toStdString());
        connect(picker, &PathPickerWidget::PathChanged, [=](const PathPickerWidget* widget, const char* newPath) {
            page->SetValue(setting->name_, newPath);
            setting->Notify();
        });
        connect(setting, &SettingValue::Changed, [=](const QVariant&) {
            picker->blockSignals(true);
            picker->SetPath(setting->value_.toString().toStdString());
            picker->blockSignals(false);
        });
        return picker;
    }
    else if (setting->type_ == ST_PATH_LIST)
    {
        PathListWidget* picker = new PathListWidget();
        picker->SetPaths(setting->value_.toStringList());

        connect(picker, &PathListWidget::PathsChanged, [=]() {
            setting->value_ = picker->GetPaths();
            setting->Notify();
        });
        connect(setting, &SettingValue::Changed, [=](const QVariant&) {
            picker->blockSignals(true);
            picker->SetPaths(setting->value_.toStringList());
            picker->blockSignals(false);
        });
        return picker;
    }
    else if (setting->type_ == ST_ENUM)
    {
        QComboBox* picker = new QComboBox();
        auto valueNames = setting->options_.toStringList();
        int index = setting->value_.toInt();
        for (auto valueName : valueNames)
            picker->addItem(Localizer::Translate(valueName));
        picker->setCurrentIndex(index);

        connect(picker, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int newIndex) {
            page->SetValue(setting->name_, newIndex);
            setting->Notify();
        });
        connect(setting, &SettingValue::Changed, [=](const QVariant&) {
            picker->blockSignals(true);
            picker->setCurrentIndex(setting->value_.toInt());
            picker->blockSignals(false);
        });
        return picker;
    }

    return 0x0;
}