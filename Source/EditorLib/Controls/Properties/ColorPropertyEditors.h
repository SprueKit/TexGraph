#pragma once

#include <EditorLib/editorlib_global.h>

#include "BasePropertyEditor.h"

class QLabel;
class QSpinBox;
class QDoubleSpinBox;

/// Property editor that works with QColor
class EDITORLIB_EXPORT ColorPropertyEditor : public BasePropertyEditor
{
    Q_OBJECT;
public:
    ColorPropertyEditor();

    virtual void SetValue(const QVariant& value) override;
    virtual QVariant GetValue() const override;

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