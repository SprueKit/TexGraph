#include "BasePropertyEditor.h"

namespace SprueEditor
{

    BasePropertyEditor::BasePropertyEditor(std::shared_ptr<PropertyEditorHandler> handler, bool vertical) :
        handler_(handler)
    {
        if (vertical)
            layout_ = new QVBoxLayout();
        else
            layout_ = new QHBoxLayout();
    }

    void BasePropertyEditor::SetEditing(std::shared_ptr<DataSource> dataSource)
    {
        editing_ = dataSource;
        PropertyUpdated(editing_);
    }

    void BasePropertyEditor::ChangePropertyEditorHandler(std::shared_ptr<PropertyEditorHandler> handler)
    {
        handler_ = handler;
        if (handler_.get())
            PropertyUpdated(editing_);
    }

    IntPropertyEditor::IntPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        spinner_ = new QSpinBox();
        layout_->addWidget(spinner_);

        spinner_->setMinimum(INT_MIN);
        spinner_->setMaximum(INT_MAX);

        PropertyUpdated(editing_);

        connect(spinner_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntPropertyEditor::SpinHandler);
    }

    void IntPropertyEditor::SpinHandler(int value)
    {
        //property_->Set(editing_, value);
        //SPRUE_PROPERTY_UNDO(value);
        if (handler_)
            handler_->CommitEditValue(this, editing_);

        emit DataChanged();
    }

    void IntPropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editing)
    {
        if ((editing_ = editing) == 0x0)
            return;
        spinner_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);
        //spinner_->setValue(property_->Get(editing->GetEditable()).getInt());

        spinner_->blockSignals(false);
    }

    BoolPropertyEditor::BoolPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        layout_->addWidget(checkbox_ = new QCheckBox());

        PropertyUpdated(editing_);

        connect(checkbox_, &QCheckBox::stateChanged, this, &BoolPropertyEditor::CheckChanged);
    }

    void BoolPropertyEditor::CheckChanged(int state)
    {
        if (handler_)
            handler_->CommitEditValue(this, editing_);
            //SPRUE_PROPERTY_UNDO(checkbox_->isChecked());
        //property_->Set(editing_, checkbox_->isChecked());
        emit DataChanged();
    }

    void BoolPropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editable)
    {
        if ((editing_ = editable) == 0x0)
        {
            checkbox_->setEnabled(false);
            return;
        }
        checkbox_->setEnabled(true);
        checkbox_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);
            //checkbox_->setChecked(property_->Get(editable->GetEditable()).getBool());

        checkbox_->blockSignals(false);
    }

    UIntPropertyEditor::UIntPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        layout_->addWidget(spinner_ = new QSpinBox());
        spinner_->setMinimum(0);
        spinner_->setMaximum(INT_MAX);

        PropertyUpdated(editing_);

        connect(spinner_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &UIntPropertyEditor::SpinChanged);
    }

    void UIntPropertyEditor::SpinChanged(int value)
    {
        if (editing_ && handler_)
        {
            handler_->CommitEditValue(this, editing_);
            //SPRUE_PROPERTY_UNDO((unsigned)value);
            //property_->Set(editing_, (unsigned)value);
            emit DataChanged();
        }
    }

    void UIntPropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editing)
    {
        if (editing == 0x0)
            return;

        spinner_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);
        //spinner_->setValue(property_->Get(editing->GetEditable()).getUInt());

        spinner_->blockSignals(false);
    }

    FloatPropertyEditor::FloatPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        layout_->addWidget(spinner_ = new QDoubleSpinBox());

        PropertyUpdated(editing_);
        spinner_->setDecimals(4);

        connect(spinner_, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &FloatPropertyEditor::SpinHandler);
    }

    void FloatPropertyEditor::SpinHandler(double value)
    {
        //property_->Set(editing_, (float)value);
        if (handler_)
            handler_->CommitEditValue(this, editing_);
        //SPRUE_PROPERTY_UNDO((float)value);
        emit DataChanged();
    }

    void FloatPropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editing)
    {
        if ((editing_ = editing) == 0x0)
            return;

        spinner_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing);
        //spinner_->setValue(property_->Get(editing->GetEditable()).getFloat());

        spinner_->blockSignals(false);
    }

    StringPropertyEditor::StringPropertyEditor(std::shared_ptr<PropertyEditorHandler> handler) :
        BasePropertyEditor(handler, false)
    {
        editor_ = new QLineEdit();
        layout_->addWidget(editor_);

        PropertyUpdated(editing_);

        connect(editor_, &QLineEdit::textChanged, this, &StringPropertyEditor::TextChanged);
    }

    void StringPropertyEditor::TextChanged(const QString& txt)
    {
        std::string newText = std::string(txt.toUtf8().constData());

        if (handler_)
            handler_->CommitEditValue(this, editing_);
        //property_->Set(editing_, newText);
        //SPRUE_PROPERTY_UNDO(newText);
        emit DataChanged();
    }

    void StringPropertyEditor::PropertyUpdated(std::shared_ptr<DataSource> editing)
    {
        if ((editing_ = editing) == 0x0)
            return;


        editor_->blockSignals(true);

        if (handler_)
            handler_->ShowEditValue(this, editing_);
        //editor_->setText(property_->Get(editing->GetEditable()).getString().c_str());

        editor_->blockSignals(false);
    }

}