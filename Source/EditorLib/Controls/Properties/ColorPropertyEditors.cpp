#include "ColorPropertyEditors.h"

#include <QColorDialog>

#include <EditorLib/Styling.h>


ColorPropertyEditor::ColorPropertyEditor() :
    BasePropertyEditor(true)
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

void ColorPropertyEditor::ApplyButtonColor()
{
    QColor col(rSpin_->value() * 255, gSpin_->value() * 255, bSpin_->value() * 255, aSpin_->value() * 255);
    QString style = QString("background-color: %1").arg(col.name());
    colorButton_->setStyleSheet(style);
}

void ColorPropertyEditor::SpinHandler(double)
{
    ApplyButtonColor();
    emit DataChanged();
    emit ValueChanged(GetValue());
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

        emit DataChanged();
        emit ValueChanged(GetValue());
    }
}

void ColorPropertyEditor::SetValue(const QVariant& value)
{
    QColor col = value.value<QColor>();
    rSpin_->blockSignals(true);
    gSpin_->blockSignals(true);
    bSpin_->blockSignals(true);
    aSpin_->blockSignals(true);

    rSpin_->setValue(col.redF());
    gSpin_->setValue(col.greenF());
    bSpin_->setValue(col.blueF());
    aSpin_->setValue(col.alphaF());

    ApplyButtonColor();

    rSpin_->blockSignals(false);
    gSpin_->blockSignals(false);
    bSpin_->blockSignals(false);
    aSpin_->blockSignals(false);
}

QVariant ColorPropertyEditor::GetValue() const
{
    QColor ret;
    ret.setRedF(rSpin_->value());
    ret.setGreenF(gSpin_->value());
    ret.setBlueF(bSpin_->value());
    ret.setAlphaF(aSpin_->value());
    return ret;
}