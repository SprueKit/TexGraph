#include "BasePropertyEditor.h"

BasePropertyEditor::BasePropertyEditor(bool vertical)
{
    if (vertical)
        layout_ = new QVBoxLayout();
    else
        layout_ = new QHBoxLayout();
}

IntPropertyEditor::IntPropertyEditor() :
    BasePropertyEditor(false)
{
    spinner_ = new QSpinBox();
    layout_->addWidget(spinner_);

    spinner_->setMinimum(INT_MIN);
    spinner_->setMaximum(INT_MAX);

    connect(spinner_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntPropertyEditor::SpinHandler);
}

void IntPropertyEditor::SpinHandler(int value)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void IntPropertyEditor::SetValue(const QVariant& value)
{
    spinner_->blockSignals(true);
    spinner_->setValue(value.toInt());
    spinner_->blockSignals(false);
}

QVariant IntPropertyEditor::GetValue() const
{
    return spinner_->value();
}

BoolPropertyEditor::BoolPropertyEditor() :
    BasePropertyEditor(false)
{
    layout_->addWidget(checkbox_ = new QCheckBox());
    connect(checkbox_, &QCheckBox::stateChanged, this, &BoolPropertyEditor::CheckChanged);
}

void BoolPropertyEditor::CheckChanged(int state)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void BoolPropertyEditor::SetValue(const QVariant& value)
{
    checkbox_->blockSignals(true);
    checkbox_->setChecked(value.toBool());
    checkbox_->blockSignals(false);
}

QVariant BoolPropertyEditor::GetValue() const
{
    return checkbox_->isChecked();
}

UIntPropertyEditor::UIntPropertyEditor() :
    BasePropertyEditor(false)
{
    layout_->addWidget(spinner_ = new QSpinBox());
    spinner_->setMinimum(0);
    spinner_->setMaximum(INT_MAX);

    connect(spinner_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &UIntPropertyEditor::SpinChanged);
}

void UIntPropertyEditor::SpinChanged(int value)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void UIntPropertyEditor::SetValue(const QVariant& value)
{
    spinner_->blockSignals(true);
    spinner_->setValue(value.toUInt());
    spinner_->blockSignals(false);
}

QVariant UIntPropertyEditor::GetValue() const
{
    return (unsigned)spinner_->value();
}

FloatPropertyEditor::FloatPropertyEditor() :
    BasePropertyEditor(false)
{
    layout_->addWidget(spinner_ = new QDoubleSpinBox());

    spinner_->setDecimals(4);
    connect(spinner_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &FloatPropertyEditor::SpinHandler);
}

void FloatPropertyEditor::SpinHandler(double value)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void FloatPropertyEditor::SetValue(const QVariant& value)
{
    spinner_->blockSignals(true);
    spinner_->setValue(value.toFloat());
    spinner_->blockSignals(false);
}

QVariant FloatPropertyEditor::GetValue() const
{
    return (float)spinner_->value();
}

StringPropertyEditor::StringPropertyEditor() :
    BasePropertyEditor(false)
{
    editor_ = new QLineEdit();
    layout_->addWidget(editor_);

    connect(editor_, &QLineEdit::textChanged, this, &StringPropertyEditor::TextChanged);
}

void StringPropertyEditor::TextChanged(const QString& txt)
{
    emit DataChanged();
    emit ValueChanged(GetValue());
}

void StringPropertyEditor::SetValue(const QVariant& value)
{
    editor_->blockSignals(true);
    editor_->setText(value.toString());
    editor_->blockSignals(false);
}

QVariant StringPropertyEditor::GetValue() const
{
    return editor_->text();
}