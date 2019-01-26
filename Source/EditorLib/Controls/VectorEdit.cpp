#include "VectorEdit.h"

#include "Styling.h"

#include <QBoxLayout>
#include <QLabel>

namespace SprueEditor
{

#define BIND_INT_SPINNER(SPINNER, INDEX) connect(SPINNER, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int val) { emit IntValueChange(INDEX, val); });
#define BIND_DOUBLE_SPINNER(SPINNER, INDEX) connect(SPINNER, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double val) { emit IntValueChange(INDEX, val); });

    VectorEdit::VectorEdit(bool integral, const char* xName, const char* yName, const char* zName, const char* wName)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setAlignment(Qt::AlignLeft);
        if (integral)
        {
            QLabel* xLbl_ = 0x0, *yLbl_ = 0x0, *zLbl_ = 0x0, *wLbl_ = 0x0;
            QSpinBox* xSpin = 0x0, *ySpin = 0x0, *zSpin = 0x0, *wSpin = 0x0;
            if (xName)
            {
                STYLING_CREATE_LBL(xLbl_, xName, STYLING_XStyle, "");
                layout->addWidget(xLbl_);
                xSpin = new QSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(xSpin);
                intSpinners_.push_back(xSpin);
                BIND_INT_SPINNER(xSpin, 0);
            }
            if (yName)
            {
                STYLING_CREATE_LBL(yLbl_, yName, STYLING_XStyle, "");
                layout->addWidget(yLbl_);
                ySpin = new QSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(ySpin);
                intSpinners_.push_back(ySpin);
                BIND_INT_SPINNER(ySpin, 1);
            }
            if (zName)
            {
                STYLING_CREATE_LBL(zLbl_, zName, STYLING_XStyle, "");
                layout->addWidget(zLbl_);
                zSpin = new QSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(zSpin);
                intSpinners_.push_back(zSpin);
                BIND_INT_SPINNER(zSpin, 2);
            }
            if (wName)
            {
                STYLING_CREATE_LBL(wLbl_, wName, STYLING_ZStyle, "");
                layout->addWidget(wLbl_);
                wSpin = new QSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(wSpin);
                intSpinners_.push_back(wSpin);
                BIND_INT_SPINNER(wSpin, 3);
            }
        }
        else
        {
            QLabel* xLbl_ = 0x0, *yLbl_ = 0x0, *zLbl_ = 0x0, *wLbl_ = 0x0;
            QDoubleSpinBox* xSpin = 0x0, *ySpin = 0x0, *zSpin = 0x0, *wSpin = 0x0;
            if (xName)
            {
                STYLING_CREATE_LBL(xLbl_, xName, STYLING_XStyle, "");
                layout->addWidget(xLbl_);
                xSpin = new QDoubleSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(xSpin);
                doubleSpinners_.push_back(xSpin);
                BIND_DOUBLE_SPINNER(xSpin, 0);
            }
            if (yName)
            {
                STYLING_CREATE_LBL(yLbl_, yName, STYLING_XStyle, "");
                layout->addWidget(yLbl_);
                ySpin = new QDoubleSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(ySpin);
                doubleSpinners_.push_back(ySpin);
                BIND_DOUBLE_SPINNER(ySpin, 1);
            }
            if (zName)
            {
                STYLING_CREATE_LBL(zLbl_, zName, STYLING_XStyle, "");
                layout->addWidget(zLbl_);
                zSpin = new QDoubleSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(zSpin);
                doubleSpinners_.push_back(zSpin);
                BIND_DOUBLE_SPINNER(zSpin, 2);
            }
            if (wName)
            {
                STYLING_CREATE_LBL(wLbl_, wName, STYLING_ZStyle, "");
                layout->addWidget(wLbl_);
                wSpin = new QDoubleSpinBox();
                STYLING_SET_SPINNER_MAX_RANGE(wSpin);
                doubleSpinners_.push_back(wSpin);
                BIND_DOUBLE_SPINNER(wSpin, 3);
            }
        }
    }

    VectorEdit::~VectorEdit()
    {

    }

    void VectorEdit::SetValue(float* data, int ct)
    {
        for (int i = 0; i < ct; ++i)
        {
            doubleSpinners_[i]->blockSignals(true);
            doubleSpinners_[i]->setValue(data[i]);
            doubleSpinners_[i]->blockSignals(false);
        }
    }

    void VectorEdit::SetValue(int* data, int ct)
    {
        for (int i = 0; i < ct; ++i)
        {
            intSpinners_[i]->blockSignals(true);
            intSpinners_[i]->setValue(data[i]);
            intSpinners_[i]->blockSignals(false);
        }
    }

}