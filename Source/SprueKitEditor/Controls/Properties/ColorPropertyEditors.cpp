#include "ColorPropertyEditors.h"

#include <QColorDialog>

#include "../../Styling.h"

namespace SprueEditor
{

    ColorPropertyEditor::ColorPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, true)
    {

        QHBoxLayout* ctrlLayout = new QHBoxLayout();

        STYLING_CREATE_LBL(rLbl_, "R", STYLING_XStyle, "");
        STYLING_CREATE_LBL(gLbl_, "G", STYLING_YStyle, "");
        STYLING_CREATE_LBL(bLbl_, "B", STYLING_ZStyle, "");
        STYLING_CREATE_LBL(aLbl_, "A", STYLING_ZStyle, "");

        rSpin_ = new QDoubleSpinBox();
        gSpin_ = new QDoubleSpinBox();
        bSpin_ = new QDoubleSpinBox();
        aSpin_ = new QDoubleSpinBox();
        
#define SET_RANGE_DBL(SPINNER) SPINNER->setSingleStep(0.01); SPINNER->setRange(0.0, 1.0);
        SET_RANGE_DBL(rSpin_);
        SET_RANGE_DBL(gSpin_);
        SET_RANGE_DBL(bSpin_);
        SET_RANGE_DBL(aSpin_);

        ctrlLayout->addWidget(rLbl_);
        ctrlLayout->addWidget(rSpin_);
        ctrlLayout->addItem(new QSpacerItem(4, 1));
        ctrlLayout->addWidget(gLbl_);
        ctrlLayout->addWidget(gSpin_);
        ctrlLayout->addItem(new QSpacerItem(4, 1));
        ctrlLayout->addWidget(bLbl_);
        ctrlLayout->addWidget(bSpin_);
        ctrlLayout->addItem(new QSpacerItem(4, 1));
        ctrlLayout->addWidget(aLbl_);
        ctrlLayout->addWidget(aSpin_);

        GetLayout()->addLayout(ctrlLayout);

        colorButton_ = new QPushButton();
        colorButton_->setMaximumWidth(128);
        GetLayout()->addWidget(colorButton_);

        //connect(colorButton, &QPushButton::clicked, this, &ColorPropertyEditor::ColorButtonHandler);

        connect(rSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ColorPropertyEditor::SpinHandler);
        connect(gSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ColorPropertyEditor::SpinHandler);
        connect(bSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ColorPropertyEditor::SpinHandler);
        connect(aSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ColorPropertyEditor::SpinHandler);
    }

    void ColorPropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editable)
    {
        if (editable == 0x0)
        {
            rSpin_->setEnabled(false);
            gSpin_->setEnabled(false);
            bSpin_->setEnabled(false);
            aSpin_->setEnabled(false);
            return;
        }

        rSpin_->setEnabled(true);
        gSpin_->setEnabled(true);
        bSpin_->setEnabled(true);
        aSpin_->setEnabled(true);
        rSpin_->blockSignals(true);
        gSpin_->blockSignals(true);
        bSpin_->blockSignals(true);
        aSpin_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);
        ApplyButtonColor();

        rSpin_->blockSignals(false);
        gSpin_->blockSignals(false);
        bSpin_->blockSignals(false);
        aSpin_->blockSignals(false);
    }

    void ColorPropertyEditor::ApplyButtonColor()
    {
        QColor col(rSpin_->value() * 255, gSpin_->value() * 255, bSpin_->value() * 255, aSpin_->value() * 255);
        QString style = QString("background-color: %1").arg(col.name());
        colorButton_->setStyleSheet(style);
    }

    void ColorPropertyEditor::SpinHandler(double)
    {
        if (handler_)
            handler_->CommitEditValue(this, editing_);
        ApplyButtonColor();
        emit DataChanged();
    }

    void ColorPropertyEditor::ColorButtonhandler()
    {
        QColor curColor(rSpin_->value() * 255, gSpin_->value() * 255, bSpin_->value() * 255, aSpin_->value() * 255);
        QColor newColor = QColorDialog::getColor(curColor);
        if (newColor.isValid())
        {
            rSpin_->blockSignals(true);
            gSpin_->blockSignals(true);
            bSpin_->blockSignals(true);
            aSpin_->blockSignals(true);

            rSpin_->setValue(newColor.redF());
            gSpin_->setValue(newColor.greenF());
            bSpin_->setValue(newColor.blueF());
            aSpin_->setValue(newColor.alphaF());

            ApplyButtonColor();

            rSpin_->blockSignals(false);
            gSpin_->blockSignals(false);
            bSpin_->blockSignals(false);
            aSpin_->blockSignals(false);

            if (handler_)
                handler_->CommitEditValue(this, editing_);
            
            emit DataChanged();
        }
    }
}