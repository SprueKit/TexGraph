#include "PermutationDlg.h"

#include "../../GlobalAccess.h"

#include <SprueEngine/IEditable.h>

#include <QtHelpers.h>

#include <EditorLib/Controls/FlagWidget.h>
#include <EditorLib/ThirdParty/QSexyToolTip.h>
#include <EditorLib/ScrollAreaWidget.h>
#include <EditorLib/Selectron.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsBitNames.h>

#include <QBoxLayout>
#include <QCheckbox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScroller>

namespace SprueEditor
{

    PermutationDlg::FieldData::~FieldData()
    {
        delete name_;
        delete weight_;
        delete flags_;
        delete editor_;
    }

    // Support Vec3, Quaternion, Float, Int, UInt, RGBA
    PermutationDlg::PermutationDlg(std::shared_ptr<IEditableDataSource> dataSource, const QString& propertyName, SprueEngine::VariantType type, SprueEngine::Permutations values) :
        QDialog(0x0),
        type_(type),
        dataSource_(dataSource),
        propertyName_(propertyName)
    {
        setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) & Qt::WindowMaximizeButtonHint);
        setWindowTitle(QString("Permutations for %1").arg(propertyName));
        setMinimumWidth(600);
        setMinimumHeight(480);

        QVBoxLayout* layout = new QVBoxLayout(this);

        QHBoxLayout* subLayout = new QHBoxLayout();
        subLayout->setAlignment(Qt::AlignTop);
        layout->addLayout(subLayout);

        ScrollAreaWidget* area = new ScrollAreaWidget();
        subLayout->addWidget(area);

        QVBoxLayout* formLayout = new QVBoxLayout();
        area->GetLayout()->addLayout(formLayout);
        formLayout->setAlignment(Qt::AlignTop);

        QPushButton* addBtn = new QPushButton(QIcon(":/Images/godot/icon_add.png"), "");
        addBtn->setToolTip("Add permutation");
        subLayout->addWidget(addBtn, 0, Qt::AlignTop);
        connect(addBtn, &QPushButton::clicked, [=](bool) {
            AddControl(formLayout);
            dirty_ = true;
        });

        for (auto item : values)
            AddControl(formLayout, &item);

        QDialogButtonBox* box = new QDialogButtonBox();
        auto okBtn = box->addButton(QDialogButtonBox::StandardButton::Ok);
        auto cancelBtn = box->addButton(QDialogButtonBox::StandardButton::Cancel);

        connect(okBtn, &QPushButton::clicked, [=](bool) {
            dataSource->GetEditable()->SetFieldPermutations(propertyName.toStdString().c_str(), GetValues());
            dirty_ = false;
            close();
        });

        auto onCloseHandler = [=]() {
            if (dirty_)
            {
                auto result = QMessageBox::warning(0x0, "Save Changes?", "Changes have been made, do you want save them?", QMessageBox::Save, QMessageBox::No);
                if (result == QMessageBox::Save)
                    dataSource->GetEditable()->SetFieldPermutations(propertyName.toStdString().c_str(), GetValues());
            }
            dirty_ = false;
            close();
        };

        connect(cancelBtn, &QPushButton::clicked, onCloseHandler);

        layout->addWidget(box);
    }

    void PermutationDlg::reject()
    {
        if (dirty_)
        {
            auto result = QMessageBox::warning(0x0, "Save Changes?", "Changes have been made, do you want save them?", QMessageBox::Save, QMessageBox::No);
            if (result == QMessageBox::Save)
                dataSource_->GetEditable()->SetFieldPermutations(propertyName_.toStdString().c_str(), GetValues());
        }
        QDialog::reject();
    }

    void PermutationDlg::AddControl(QVBoxLayout* holder, SprueEngine::FieldPermutation* val)
    {
        QWidget* widget = 0x0;// CreateEditorFor(type_);

        QHBoxLayout* masterLayout = new QHBoxLayout();
        holder->addLayout(masterLayout);

        QVBoxLayout* ctrlLayout = new QVBoxLayout();
        masterLayout->addLayout(ctrlLayout);

        FieldData* data = new FieldData();

    // Extra widgets
        {
            QHBoxLayout* stuffLayout = new QHBoxLayout();
            stuffLayout->setAlignment(Qt::AlignLeft);
            ctrlLayout->addLayout(stuffLayout);
            QLineEdit* nameEdit = data->name_ = new QLineEdit();
            if (val)
                nameEdit->setText(val->permutationName_.c_str());

            QLineEdit* weightEdit = data->weight_ = new QLineEdit();
            weightEdit->setValidator(new QIntValidator(1, 100));
            if (val)
                weightEdit->setText(QString::number(val->permutationWeight_));
            else
                weightEdit->setText("1");
            weightEdit->setMaximumWidth(80);
            stuffLayout->addWidget(new QLabel("Name"));
            stuffLayout->addWidget(nameEdit);
            stuffLayout->addWidget(new QLabel("Weight"));
            stuffLayout->addWidget(weightEdit);

            QHBoxLayout* flagLayout = new QHBoxLayout();
            flagLayout->setAlignment(Qt::AlignLeft);
            ctrlLayout->addLayout(flagLayout);
            FlagWidget* flagCtrl = data->flags_ = new FlagWidget();
            flagCtrl->SetBitCount(32);
            flagCtrl->SetBitField(0);
            if (val)
                flagCtrl->SetBitField(val->permutationFlags_);
            flagLayout->addWidget(new QLabel("Flags"));
            flagLayout->addWidget(flagCtrl, 0, Qt::AlignLeft);

            QPushButton* popupButton = new QPushButton(QIcon(":/qss_icons/rc/down_arrow.png"), "");
            popupButton->setMaximumSize(20, 20);
            flagLayout->addWidget(popupButton, 0, Qt::AlignLeft);

            auto bitNames = Settings::GetInstance()->GetBitNames("Permutation Flags");// "User Flags");
            
            // Fill flags popup
            {
                QSexyToolTip* flagsPopup = new QSexyToolTip();
                flagsPopup->attach(popupButton);
                QVBoxLayout* flagsLayout = new QVBoxLayout(flagsPopup);
                ScrollAreaWidget* widget = new ScrollAreaWidget();
                widget->setMinimumHeight(300);
                flagsLayout->addWidget(widget);
                for (unsigned i = 0; i < 32; ++i)
                {
                    QString bitLabel = bitNames ? (*bitNames)[i] : "";
                    if (bitLabel.isEmpty())
                        bitLabel = QString("Bit %1").arg(i + 1);
                    data->flags_->setToolTip(bitLabel);

                    QCheckBox* flagCheck = new QCheckBox(bitLabel);
                    widget->GetLayout()->addWidget(flagCheck);
                    connect(flagCheck, &QCheckBox::toggled, [=](bool state) {
                        flagCheck->blockSignals(true);
                        data->flags_->SetBit(i, state);
                        flagCheck->blockSignals(false);
                    });
                    connect(data->flags_, &FlagWidget::BitFieldChanged, [=]() {
                        flagCheck->blockSignals(true);
                        flagCheck->setChecked((1 << i) & data->flags_->GetBitField());
                        flagCheck->blockSignals(false);
                    });
                }
            }
        }

    // Variant editor
        {
#if 0
            QHBoxLayout* rowLayout = new QHBoxLayout();
            ctrlLayout->addLayout(rowLayout);

            if (val)
                widget->SetValue(val->value_);

            data->editor_ = widget;
            connect(widget, &BaseTypeEditor::DataChanged, [=]() {
                dirty_ = true;
            });
            rowLayout->addLayout(widget->GetLayout());
#endif
        }

    // Delete button
        {
            QVBoxLayout* btnsLayout = new QVBoxLayout();

            QPushButton* delBtn = new QPushButton(QIcon(":/Images/godot/icon_remove.png"), "");
            delBtn->setToolTip("Delete");
            connect(delBtn, &QPushButton::clicked, [=](bool) {
                holder->removeItem(masterLayout);
                auto found = std::find(editors_.begin(), editors_.end(), data);
                if (found != editors_.end())
                {
                    auto ff = *found;
                    editors_.erase(found);
                    QtHelpers::ClearLayout(masterLayout);
                    masterLayout->deleteLater();
                    delete ff;
                }
            });
            
#if 0
            QPushButton* setBtn = new QPushButton(QIcon(":/Images/fugue/ticket--arrow.png"), "");
            setBtn->setToolTip("Use this value");
            connect(setBtn, &QPushButton::clicked, [=](bool) {
                dataSource_->GetEditable()->SetProperty(propertyName_.toStdString(), widget->GetValue());
                Global_ObjectSectron()->NotifyDataChanged(0x0, dataSource_, SprueEngine::StringHash(propertyName_.toStdString()).value_);
            });

            btnsLayout->addWidget(delBtn, 0, Qt::AlignTop);
            btnsLayout->addWidget(setBtn, 0, Qt::AlignBottom);
            masterLayout->addLayout(btnsLayout);
#endif
        }

    // Horizontal Rule seperation
        QFrame* hFrame = new QFrame;
        hFrame->setFrameShape(QFrame::HLine);
        ctrlLayout->addWidget(hFrame);

        editors_.push_back(data);
    }

    PermutationDlg::~PermutationDlg()
    {

    }

    SprueEngine::Permutations PermutationDlg::GetValues()
    {
        SprueEngine::Permutations ret;
        for (auto editor : editors_)
        {
#if 0
            SprueEngine::FieldPermutation perm;
            perm.permutationName_ = editor->name_->text().toStdString().c_str();
            perm.permutationWeight_ = editor->weight_->text().toInt();
            perm.permutationFlags_ = editor->flags_->GetBitField();
            perm.value_ = editor->editor_->GetValue();
            ret.push_back(perm);
#endif
        }
        return ret;
    }
}