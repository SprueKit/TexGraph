#include "SpruePropertyHandlerImpl.h"

#include "../../../GlobalAccess.h"
#include "../BasePropertyEditor.h"
#include "../VectorPropertyEditors.h"

#include "../../../Commands/SprueKit/IEditablePropertyCmd.h"
#include "../../../Data/SprueDataSources.h"
#include "../../../SprueKitEditor.h"

#include <SprueEngine/IEditable.h>
#include <SprueEngine/Property.h>

#define SPRUE_PROPERTY_UNDO(VALUE) { SmartCommand* cmd = new IEditablePropertyCommand(data, property, property->Get(editing), VALUE, hash_.value_ != 0 ? hash_ : property->GetHash()); \
    Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd); }

namespace SprueEditor
{

    bool SprueBasePropertyHandler::IsPropertyDirty(std::shared_ptr<DataSource> dataSource)
    {
        if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(dataSource))
            if (auto edit = data->GetEditable())
            {
                if (auto prop = edit->FindProperty(hash_))
                    return prop->Get(edit) != prop->GetDefault();
            }
        return false;
    }

// Boolean
    void SprueBoolPropertyHandler::ShowEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        BoolPropertyEditor* editor = (BoolPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            if (auto editing_ = data->GetEditable())
            {
                if (auto prop = editing_->FindProperty(hash_))
                    editor->checkbox_->setChecked(prop->Get(editing_).getBool());
            }
    }
    void SprueBoolPropertyHandler::CommitEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        BoolPropertyEditor* editor = (BoolPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            if (auto editing = data->GetEditable())
            {
                if (auto property = editing->FindProperty(hash_))
                    SPRUE_PROPERTY_UNDO(editor->checkbox_->isChecked());
            }
    }

// Vector3
    void SprueVec3PropertyHandler::ConfigureEditor(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        if (auto editor = (Vector3PropertyEditor*)aEditor)
        {
            if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            {
                if (auto editing = data->GetEditable())
                {
                    if (auto property = editing->FindProperty(hash_))
                    {
                        if (property->GetFlags() & SprueEngine::PS_TinyIncrement)
                        {
                            editor->xSpin_->setSingleStep(0.01);
                            editor->ySpin_->setSingleStep(0.01);
                            editor->zSpin_->setSingleStep(0.01);
                        }
                        else if (property->GetFlags() & SprueEngine::PS_SmallIncrement)
                        {
                            editor->xSpin_->setSingleStep(0.1);
                            editor->ySpin_->setSingleStep(0.1);
                            editor->zSpin_->setSingleStep(0.1);
                        }
                    }
                }
            }
        }
    }
    void SprueVec3PropertyHandler::ShowEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        Vector3PropertyEditor* editor = (Vector3PropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            if (auto editing = data->GetEditable())
            {
                if (auto prop = editing->FindProperty(hash_))
                {
                    SprueEngine::Vec3 val = prop->Get(editing).getVec3();
                    editor->xSpin_->setValue(val.x);
                    editor->ySpin_->setValue(val.y);
                    editor->zSpin_->setValue(val.z);
                }
            }
    }
    void SprueVec3PropertyHandler::CommitEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        Vector3PropertyEditor* editor = (Vector3PropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            if (auto editing = data->GetEditable())
            {
                if (auto property = editing->FindProperty(hash_))
                {
                    SprueEngine::Vec3 val;
                    val.x = editor->xSpin_->value();
                    val.y = editor->ySpin_->value();
                    val.z = editor->zSpin_->value();
                    SPRUE_PROPERTY_UNDO(val);
                }
            }
    }

// Quaternion
    void SprueQuatPropertyHandler::ConfigureEditor(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        if (auto editor = (QuatPropertyEditor*)aEditor)
        {
            if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            {
                if (auto editing = data->GetEditable())
                {
                    if (auto property = editing->FindProperty(hash_))
                    {
                        if (property->GetFlags() & SprueEngine::PS_TinyIncrement)
                        {
                            editor->xSpin_->setSingleStep(0.01);
                            editor->ySpin_->setSingleStep(0.01);
                            editor->zSpin_->setSingleStep(0.01);
                        }
                        else if (property->GetFlags() & SprueEngine::PS_SmallIncrement)
                        {
                            editor->xSpin_->setSingleStep(0.1);
                            editor->ySpin_->setSingleStep(0.1);
                            editor->zSpin_->setSingleStep(0.1);
                        }
                    }
                }
            }
        }
    }
    void SprueQuatPropertyHandler::ShowEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        QuatPropertyEditor* editor = (QuatPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            if (auto editing = data->GetEditable())
            {
                if (auto prop = editing->FindProperty(hash_))
                {
                    SprueEngine::Quat val = prop->Get(editing).getQuat();
                    SprueEngine::Vec3 vec = val.ToEulerXYZ();
                    editor->xSpin_->setValue(vec.x * RAD_TO_DEG);
                    editor->ySpin_->setValue(vec.y * RAD_TO_DEG);
                    editor->zSpin_->setValue(vec.z * RAD_TO_DEG);
                }
            }
    }
    void SprueQuatPropertyHandler::CommitEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        QuatPropertyEditor* editor = (QuatPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<IEditableDataSource>(datasource))
            if (auto editing = data->GetEditable())
            {
                if (auto property = editing->FindProperty(hash_))
                {
                    SprueEngine::Quat val = SprueEngine::Quat::FromEulerXYZ(editor->xSpin_->value() * DEG_TO_RAD, editor->ySpin_->value() * DEG_TO_RAD, editor->zSpin_->value() * DEG_TO_RAD);
                    SPRUE_PROPERTY_UNDO(val);
                }
            }
    }
}