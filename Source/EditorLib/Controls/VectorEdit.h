#pragma once

#include <QWidget>
#include <QSpinBox>

namespace SprueEditor
{

    class VectorEdit : public QWidget
    {
        Q_OBJECT;
    public:
        VectorEdit(bool integral, const char* xName, const char* yName = 0x0, const char* zName = 0x0, const char* wName = 0x0);
        virtual ~VectorEdit();

        void SetValue(float* data, int ct);
        void SetValue(int* data, int ct);

    signals:
        void FloatValueChange(int index, float value);
        void IntValueChange(int index, int value);

    private:
        std::vector<QSpinBox*> intSpinners_;
        std::vector<QDoubleSpinBox*> doubleSpinners_;
    };

}