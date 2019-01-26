#pragma once

#include "BasePropertyEditor.h"

class QLabel;
class QSpinBox;
class QDoubleSpinBox;

namespace SprueEditor
{

    class ColorPropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        ColorPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>);

        QLabel* rLbl_;
        QLabel* gLbl_;
        QLabel* bLbl_;
        QLabel* aLbl_;
        QDoubleSpinBox* rSpin_;
        QDoubleSpinBox* gSpin_;
        QDoubleSpinBox* bSpin_;
        QDoubleSpinBox* aSpin_;
        QPushButton* colorButton_;

        void ApplyButtonColor();

    public slots:
        void SpinHandler(double);
        void ColorButtonhandler();
    };   

}