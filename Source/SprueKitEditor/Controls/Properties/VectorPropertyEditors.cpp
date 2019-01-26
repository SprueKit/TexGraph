#include "VectorPropertyEditors.h"

#include <QSpinBox>

namespace SprueEditor
{

#define SET_RANGE(SPINNER) SPINNER->setMinimum(INT_MIN); SPINNER->setMaximum(INT_MAX);
#define SET_RANGE_DBL(SPINNER) SPINNER->setMinimum(INT_MIN); SPINNER->setMaximum(INT_MAX); SPINNER->setDecimals(4);

    //SPINNER->setMaximumWidth(80);

#define XStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #DD0000; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define YStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #00AA00; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define ZStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #3F3FDD; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"
#define AStyle "QLabel { color: white; font-size: 8pt; font-weight: bold; background-color: #AA007F; border-top-left-radius: 5px; border-bottom-left-radius: 5px; border-bottom-right-radius: 0px; border-top-right-radius: 0px; }"

#define CREATE_LBL(VARNAME, TEXT, STYLE, TIP) VARNAME = new QLabel(" " TEXT); \
VARNAME->setMaximumWidth(18); \
VARNAME->setMinimumWidth(18); \
VARNAME->setMaximumHeight(18); \
VARNAME->setStyleSheet(STYLE); \
VARNAME->setAlignment(Qt::AlignCenter); \
VARNAME->setToolTip(TIP);

#define CREATE_LBL_SIZED(VARNAME, TEXT, STYLE, SIZE, TIP) VARNAME = new QLabel(" " TEXT); \
VARNAME->setMaximumWidth(SIZE); \
VARNAME->setMinimumWidth(SIZE); \
VARNAME->setMaximumHeight(18); \
VARNAME->setStyleSheet(STYLE); \
VARNAME->setAlignment(Qt::AlignCenter); \
VARNAME->setToolTip(TIP);

    IntVector2PropertyEditor::IntVector2PropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        layout_->setSpacing(0);
        layout_->setAlignment(Qt::AlignLeft);
        CREATE_LBL(xLbl_, "X", XStyle, "");
        CREATE_LBL(yLbl_, "Y", YStyle, "");

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
        //IntVec2 val(xSpin_->value(), ySpin_->value());
        ////property_->Set(editing_, val);
        //if (property_)
        //    SPRUE_PROPERTY_UNDO(val);
        if (handler_)
            handler_->CommitEditValue(this, editing_);
        emit DataChanged();
    }

    void IntVector2PropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editable)
    {
        if (editable == 0x0)
        {
            xSpin_->setEnabled(false);
            ySpin_->setEnabled(false);
            return;
        }

        xSpin_->setEnabled(true);
        ySpin_->setEnabled(true);
        xSpin_->blockSignals(true);
        ySpin_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);
        //if (property_)
        //{
        //    IntVec2 vec = property_->Get(editable->GetEditable()).getIntVec2();
        //    xSpin_->setValue(vec.x);
        //    ySpin_->setValue(vec.y);
        //}

        xSpin_->blockSignals(false);
        ySpin_->blockSignals(false);
    }

    Vector2PropertyEditor::Vector2PropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        layout_->setSpacing(0);
        layout_->setAlignment(Qt::AlignLeft);
        CREATE_LBL(xLbl_, "X", XStyle, "");
        CREATE_LBL(yLbl_, "Y", YStyle, "");

        xSpin_ = new QDoubleSpinBox();
        ySpin_ = new QDoubleSpinBox();

        SET_RANGE_DBL(xSpin_);
        SET_RANGE_DBL(ySpin_);

        //if (property)
        //{
        //    if (property->GetFlags() & PS_TinyIncrement)
        //    {
        //        xSpin_->setSingleStep(0.01);
        //        ySpin_->setSingleStep(0.01);
        //    }
        //    else if (property->GetFlags() & PS_SmallIncrement)
        //    {
        //        xSpin_->setSingleStep(0.1);
        //        ySpin_->setSingleStep(0.1);
        //    }
        //}

        layout_->addWidget(xLbl_);
        layout_->addWidget(xSpin_);
        layout_->addItem(new QSpacerItem(4, 1));
        layout_->addWidget(yLbl_);
        layout_->addWidget(ySpin_);

        PropertyUpdated(editing_);

        connect(xSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector2PropertyEditor::SpinHandler);
        connect(ySpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector2PropertyEditor::SpinHandler);
    }

    void Vector2PropertyEditor::SpinHandler(double)
    {
        if (handler_)
            handler_->CommitEditValue(this, editing_);
        emit DataChanged();
    }

    void Vector2PropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editable)
    {
        if (editable == 0x0)
        {
            xSpin_->setEnabled(false);
            ySpin_->setEnabled(false);
            return;
        }

        xSpin_->setEnabled(true);
        ySpin_->setEnabled(true);
        xSpin_->blockSignals(true);
        ySpin_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);

        xSpin_->blockSignals(false);
        ySpin_->blockSignals(false);
    }

    Vector3PropertyEditor::Vector3PropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        layout_->setSpacing(0);
        layout_->setAlignment(Qt::AlignLeft);
        CREATE_LBL(xLbl_, "X", XStyle, "");
        CREATE_LBL(yLbl_, "Y", YStyle, "");
        CREATE_LBL(zLbl_, "Z", ZStyle, "");

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

        PropertyUpdated(editing_);

        connect(xSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector3PropertyEditor::SpinHandler);
        connect(ySpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector3PropertyEditor::SpinHandler);
        connect(zSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Vector3PropertyEditor::SpinHandler);
    }

    void Vector3PropertyEditor::SpinHandler(double)
    {
        if (handler_)
            handler_->CommitEditValue(this, editing_);
        emit DataChanged();
    }

    void Vector3PropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editable)
    {
        if (editable == 0x0)
        {
            xSpin_->setEnabled(false);
            ySpin_->setEnabled(false);
            zSpin_->setEnabled(false);
            return;
        }

        xSpin_->setEnabled(true);
        ySpin_->setEnabled(true);
        zSpin_->setEnabled(true);
        xSpin_->blockSignals(true);
        ySpin_->blockSignals(true);
        zSpin_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);
        //if (property_)
        //{
        //    Vec3 vec = property_->Get(editable->GetEditable()).getVec3();
        //    xSpin_->setValue(vec.x);
        //    ySpin_->setValue(vec.y);
        //    zSpin_->setValue(vec.z);
        //}

        xSpin_->blockSignals(false);
        ySpin_->blockSignals(false);
        zSpin_->blockSignals(false);
    }

    QuatPropertyEditor::QuatPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        layout_->setSpacing(0);
        layout_->setAlignment(Qt::AlignLeft);
        CREATE_LBL(xLbl_, "X", XStyle, "");
        CREATE_LBL(yLbl_, "Y", YStyle, "");
        CREATE_LBL(zLbl_, "Z", ZStyle, "");

        xSpin_ = new QDoubleSpinBox();
        ySpin_ = new QDoubleSpinBox();
        zSpin_ = new QDoubleSpinBox();

        SET_RANGE_DBL(xSpin_);
        SET_RANGE_DBL(ySpin_);
        SET_RANGE_DBL(zSpin_);

        //if (property)
        //{
        //    if (property->GetFlags() & PS_TinyIncrement)
        //    {
        //        xSpin_->setSingleStep(0.01);
        //        ySpin_->setSingleStep(0.01);
        //        zSpin_->setSingleStep(0.01);
        //    }
        //    else if (property->GetFlags() & PS_SmallIncrement)
        //    {
        //        xSpin_->setSingleStep(0.1);
        //        ySpin_->setSingleStep(0.1);
        //        zSpin_->setSingleStep(0.1);
        //    }
        //}

        layout_->addWidget(xLbl_);
        layout_->addWidget(xSpin_);
        layout_->addItem(new QSpacerItem(4, 1));
        layout_->addWidget(yLbl_);
        layout_->addWidget(ySpin_);
        layout_->addItem(new QSpacerItem(4, 1));
        layout_->addWidget(zLbl_);
        layout_->addWidget(zSpin_);

        PropertyUpdated(editing_);

        connect(xSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QuatPropertyEditor::SpinHandler);
        connect(ySpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QuatPropertyEditor::SpinHandler);
        connect(zSpin_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QuatPropertyEditor::SpinHandler);
    }

    void QuatPropertyEditor::SpinHandler(double)
    {
        if (handler_)
            handler_->CommitEditValue(this, editing_);
        emit DataChanged();
    }

    void QuatPropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editable)
    {
        if (editable == 0x0)
        {
            xSpin_->setEnabled(false);
            ySpin_->setEnabled(false);
            zSpin_->setEnabled(false);
            return;
        }

        xSpin_->setEnabled(true);
        ySpin_->setEnabled(true);
        zSpin_->setEnabled(true);
        xSpin_->blockSignals(true);
        ySpin_->blockSignals(true);
        zSpin_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);

        xSpin_->blockSignals(false);
        ySpin_->blockSignals(false);
        zSpin_->blockSignals(false);
    }

}