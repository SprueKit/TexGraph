#pragma once

#include "BasePropertyEditor.h"

class QLabel;
class QSpinBox;
class QDoubleSpinBox;

namespace SprueEditor
{

    class IntVector2PropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        IntVector2PropertyEditor(std::shared_ptr<PropertyEditorHandler> handler);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>);

        QLabel* xLbl_;
        QLabel* yLbl_;
        QSpinBox* xSpin_;
        QSpinBox* ySpin_;

    public slots:
        void SpinHandler(int);
    };

    class Vector2PropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        Vector2PropertyEditor(std::shared_ptr<PropertyEditorHandler> handler);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>);

        QLabel* xLbl_;
        QLabel* yLbl_;
        QDoubleSpinBox* xSpin_;
        QDoubleSpinBox* ySpin_;

    public slots:
        void SpinHandler(double);
    };

    class Vector3PropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        Vector3PropertyEditor(std::shared_ptr<PropertyEditorHandler> handler);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>);

        QLabel* xLbl_;
        QLabel* yLbl_;
        QLabel* zLbl_;
        QDoubleSpinBox* xSpin_;
        QDoubleSpinBox* ySpin_;
        QDoubleSpinBox* zSpin_;

    public slots:
        void SpinHandler(double);
    };

    class QuatPropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        QuatPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>);

        QLabel* xLbl_;
        QLabel* yLbl_;
        QLabel* zLbl_;
        QDoubleSpinBox* xSpin_;
        QDoubleSpinBox* ySpin_;
        QDoubleSpinBox* zSpin_;

    public slots:
        void SpinHandler(double);
    };

}