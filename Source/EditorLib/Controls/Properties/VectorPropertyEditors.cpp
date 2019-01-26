#include "VectorPropertyEditors.h"

#include <QSpinBox>
#include <QVector2d>
#include <QVector3d>
#include <QQuaternion>

#include <EditorLib/Styling.h>

#define SET_RANGE(SPINNER) SPINNER->setMinimum(INT_MIN); SPINNER->setMaximum(INT_MAX);
#define SET_RANGE_DBL(SPINNER) SPINNER->setMinimum(INT_MIN); SPINNER->setMaximum(INT_MAX); SPINNER->setDecimals(4);
//
////SPINNER->setMaximumWidth(80);
//
//#define XStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #DD0000; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
//#define YStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #00AA00; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
//#define ZStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #3F3FDD; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
//#define AStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #AA007F; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
//
//#define CREATE_LBL(VARNAME, TEXT, STYLE, TIP) VARNAME = new QLabel(" " TEXT); \
//VARNAME->setMaximumWidth(18); \
//VARNAME->setMinimumWidth(18); \
//VARNAME->setMaximumHeight(18); \
//VARNAME->setStyleSheet(STYLE); \
//VARNAME->setAlignment(Qt::AlignCenter); \
//VARNAME->setToolTip(TIP);
//
//#define CREATE_LBL_SIZED(VARNAME, TEXT, STYLE, SIZE, TIP) VARNAME = new QLabel(" " TEXT); \
//VARNAME->setMaximumWidth(SIZE); \
//VARNAME->setMinimumWidth(SIZE); \
//VARNAME->setMaximumHeight(18); \
//VARNAME->setStyleSheet(STYLE); \
//VARNAME->setAlignment(Qt::AlignCenter); \
//VARNAME->setToolTip(TIP);

IntVector2PropertyEditor::IntVector2PropertyEditor() :
    BasePropertyEditor(false)
{
    layout_->setSpacing(0);
    layout_->setAlignment(Qt::AlignLeft);
    STYLING_CREATE_LBL(xLbl_, "X", STYLING_XStyle, "");
    STYLING_CREATE_LBL(yLbl_, "Y", STYLING_YStyle, "");

    xSpin_ = new QSpinBox();
    ySpin_ = new QSpinBox();

    SET_RANGE(xSpin_);
    SET_RANGE(ySpin_);

    layout_->addWidget(xLbl_);
    layout_->addWidget(xSpin_);
    layout_->addItem(new QSpacerItem(4, 1));
    layout_->addWidget(yLbl_);
    layout_->addWidget(ySpin_);

    connect(xSpin_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntVector2PropertyEditor::SpinHandler);
    connect(ySpin_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntVector2PropertyEditor::SpinHandler);
}

void IntVector2PropertyEditor::SpinHandler(int value)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void IntVector2PropertyEditor::SetValue(const QVariant& value)
{
    QPoint pt = value.value<QPoint>();
    xSpin_->blockSignals(true);
    ySpin_->blockSignals(true);

    xSpin_->setValue(pt.x());
    ySpin_->setValue(pt.y());

    xSpin_->blockSignals(false);
    ySpin_->blockSignals(false);
}

QVariant IntVector2PropertyEditor::GetValue() const
{
    QVariant ret;
    ret.setValue(QPoint(xSpin_->value(), ySpin_->value()));
    return ret;
}

Vector2PropertyEditor::Vector2PropertyEditor() :
    BasePropertyEditor(false)
{
    layout_->setSpacing(0);
    layout_->setAlignment(Qt::AlignLeft);
    STYLING_CREATE_LBL(xLbl_, "X", STYLING_XStyle, "");
    STYLING_CREATE_LBL(yLbl_, "Y", STYLING_YStyle, "");

    xSpin_ = new QDoubleSpinBox();
    ySpin_ = new QDoubleSpinBox();

    SET_RANGE_DBL(xSpin_);
    SET_RANGE_DBL(ySpin_);
    
    layout_->addWidget(xLbl_);
    layout_->addWidget(xSpin_);
    layout_->addItem(new QSpacerItem(4, 1));
    layout_->addWidget(yLbl_);
    layout_->addWidget(ySpin_);

    connect(xSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector2PropertyEditor::SpinHandler);
    connect(ySpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector2PropertyEditor::SpinHandler);
}

void Vector2PropertyEditor::SpinHandler(double)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void Vector2PropertyEditor::SetValue(const QVariant& value)
{
    QVector2D vec = value.value<QVector2D>();
    xSpin_->blockSignals(true);
    ySpin_->blockSignals(true);

    xSpin_->setValue(vec.x());
    ySpin_->setValue(vec.y());

    xSpin_->blockSignals(false);
    ySpin_->blockSignals(false);
}

QVariant Vector2PropertyEditor::GetValue() const
{
    QVariant ret;
    ret.setValue(QVector2D(xSpin_->value(), ySpin_->value()));
    return ret;
}

Vector3PropertyEditor::Vector3PropertyEditor() :
    BasePropertyEditor(false)
{
    layout_->setSpacing(0);
    layout_->setAlignment(Qt::AlignLeft);
    STYLING_CREATE_LBL(xLbl_, "X", STYLING_XStyle, "");
    STYLING_CREATE_LBL(yLbl_, "Y", STYLING_YStyle, "");
    STYLING_CREATE_LBL(zLbl_, "Z", STYLING_ZStyle, "");

    xSpin_ = new QDoubleSpinBox();
    ySpin_ = new QDoubleSpinBox();
    zSpin_ = new QDoubleSpinBox();

    SET_RANGE_DBL(xSpin_);
    SET_RANGE_DBL(ySpin_);
    SET_RANGE_DBL(zSpin_);

    layout_->addWidget(xLbl_);
    layout_->addWidget(xSpin_);
    layout_->addItem(new QSpacerItem(4, 1));
    layout_->addWidget(yLbl_);
    layout_->addWidget(ySpin_);
    layout_->addItem(new QSpacerItem(4, 1));
    layout_->addWidget(zLbl_);
    layout_->addWidget(zSpin_);

    connect(xSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector3PropertyEditor::SpinHandler);
    connect(ySpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector3PropertyEditor::SpinHandler);
    connect(zSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector3PropertyEditor::SpinHandler);
}

void Vector3PropertyEditor::SpinHandler(double)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void Vector3PropertyEditor::SetValue(const QVariant& value)
{
    xSpin_->blockSignals(true);
    ySpin_->blockSignals(true);
    zSpin_->blockSignals(true);

    QVector3D vec = value.value<QVector3D>();
    xSpin_->setValue(vec.x());
    ySpin_->setValue(vec.y());
    zSpin_->setValue(vec.z());

    xSpin_->blockSignals(false);
    ySpin_->blockSignals(false);
    zSpin_->blockSignals(false);
}

QVariant Vector3PropertyEditor::GetValue() const
{
    QVariant ret;
    ret.setValue(QVector3D(xSpin_->value(), ySpin_->value(), zSpin_->value()));
    return ret;
}

QuatPropertyEditor::QuatPropertyEditor() :
    BasePropertyEditor(false)
{
    layout_->setSpacing(0);
    layout_->setAlignment(Qt::AlignLeft);
    STYLING_CREATE_LBL(xLbl_, "X", STYLING_XStyle, "");
    STYLING_CREATE_LBL(yLbl_, "Y", STYLING_YStyle, "");
    STYLING_CREATE_LBL(zLbl_, "Z", STYLING_ZStyle, "");

    xSpin_ = new QDoubleSpinBox();
    ySpin_ = new QDoubleSpinBox();
    zSpin_ = new QDoubleSpinBox();

    SET_RANGE_DBL(xSpin_);
    SET_RANGE_DBL(ySpin_);
    SET_RANGE_DBL(zSpin_);

    layout_->addWidget(xLbl_);
    layout_->addWidget(xSpin_);
    layout_->addItem(new QSpacerItem(4, 1));
    layout_->addWidget(yLbl_);
    layout_->addWidget(ySpin_);
    layout_->addItem(new QSpacerItem(4, 1));
    layout_->addWidget(zLbl_);
    layout_->addWidget(zSpin_);

    connect(xSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QuatPropertyEditor::SpinHandler);
    connect(ySpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QuatPropertyEditor::SpinHandler);
    connect(zSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QuatPropertyEditor::SpinHandler);
}

void QuatPropertyEditor::SpinHandler(double)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void QuatPropertyEditor::SetValue(const QVariant& value)
{
    xSpin_->blockSignals(true);
    ySpin_->blockSignals(true);
    zSpin_->blockSignals(true);

    QQuaternion quat = value.value<QQuaternion>();
    auto vec = quat.toEulerAngles();
    xSpin_->setValue(vec.x());
    ySpin_->setValue(vec.y());
    zSpin_->setValue(vec.z());

    xSpin_->blockSignals(false);
    ySpin_->blockSignals(false);
    zSpin_->blockSignals(false);
}

QVariant QuatPropertyEditor::GetValue() const
{
    QVariant ret;
    ret.setValue(QQuaternion::fromEulerAngles(xSpin_->value(), ySpin_->value(), zSpin_->value()));
    return ret;
}