#include "UrhoPropertyHandlerImpl.h"

#include "../BasePropertyEditor.h"
#include "../ColorPropertyEditors.h"
#include "../VectorPropertyEditors.h"

#include "../../../Commands/Urho/UrhoPropertyCmd.h"
#include "../../../Data/UrhoDataSources.h"
#include "../../../GlobalAccess.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Commands/SmartUndoStack.h>

using namespace SprueEditor;

#define URHO_PROPERTY_UNDO(EDITING, PROPERTY, NEWVALUE) { SmartCommand* cmd = new UrhoEditor::UrhoPropertyCmd(EDITING, PROPERTY, NEWVALUE, EDITING->GetObject()->GetAttribute(name_)); \
    Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd); }

namespace UrhoEditor
{

    const Urho3D::AttributeInfo* UrhoBasePropertyHandler::Attribute(Urho3D::Serializable* serialziable)
    {
        auto attrs = serialziable->GetAttributes();
        for (unsigned i = 0; i < attrs->Size(); ++i)
        {
            if ((*attrs)[i].name_ == name_)
                return &((*attrs)[i]);
        }
        return 0x0;
    }

    bool UrhoBasePropertyHandler::IsPropertyDirty(std::shared_ptr<DataSource> dataSource)
    {
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(dataSource))
            if (auto edit = data->GetObject())
            {
                if (auto prop = Attribute(edit.Get()))
                {
                    Urho3D::Variant curVal;
                    prop->accessor_->Get(edit.Get(), curVal);
                    return prop->defaultValue_ == curVal;
                }
            }
        return false;
    }

    // Boolean
    void UrhoBoolPropertyHandler::ShowEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        BoolPropertyEditor* editor = (BoolPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(datasource))
            if (auto editing_ = data->GetObject())
            {
                if (auto prop = Attribute(editing_.Get()))
                {
                    Urho3D::Variant curVal;
                    prop->accessor_->Get(editing_.Get(), curVal);
                    editor->checkbox_->setChecked(curVal.GetBool());
                }
            }
    }
    void UrhoBoolPropertyHandler::CommitEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        BoolPropertyEditor* editor = (BoolPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(datasource))
            if (auto editing = data->GetObject())
            {
                if (auto property = Attribute(editing.Get()))
                {
                    bool value = editor->checkbox_->isChecked();
                    //SprueEditor::SprueCommand* cmd = new UrhoEditor::UrhoPropertyCmd(data, hash_.Value(), value, data->GetObject()->GetAttribute(name_));
                    //SprueEditor::SprueKitEditor::GetInstance()->GetDocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd);
                    URHO_PROPERTY_UNDO(data, hash_.Value(), value);
                }
            }
    }

    /// Color
    void UrhoColorPropertyHandler::ConfigureEditor(SprueEditor::BasePropertyEditor* editor, std::shared_ptr<DataSource> editable)
    {

    }
    
    void UrhoColorPropertyHandler::ShowEditValue(SprueEditor::BasePropertyEditor* aEditor, std::shared_ptr<DataSource> dataSource)
    {
        ColorPropertyEditor* editor = (ColorPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(dataSource))
            if (auto editing_ = data->GetObject())
            {
                if (auto prop = Attribute(editing_.Get()))
                {
                    Urho3D::Variant curVal;
                    prop->accessor_->Get(editing_.Get(), curVal);
                    editor->rSpin_->setValue(curVal.GetColor().r_);
                    editor->gSpin_->setValue(curVal.GetColor().g_);
                    editor->bSpin_->setValue(curVal.GetColor().b_);
                    editor->aSpin_->setValue(curVal.GetColor().a_);
                }
            }
    }

    void UrhoColorPropertyHandler::CommitEditValue(SprueEditor::BasePropertyEditor* aEditor, std::shared_ptr<DataSource> dataSource)
    {
        ColorPropertyEditor* editor = (ColorPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(dataSource))
            if (auto editing = data->GetObject())
            {
                if (auto property = Attribute(editing.Get()))
                {
                    Urho3D::Color value;
                    value.r_ = editor->rSpin_->value();
                    value.g_ = editor->gSpin_->value();
                    value.b_ = editor->bSpin_->value();
                    value.a_ = editor->aSpin_->value();
                    editor->ApplyButtonColor();
                    URHO_PROPERTY_UNDO(data, hash_.Value(), value);
                }
            }
    }

    /// Vector2
    void UrhoVec2PropertyHandler::ConfigureEditor(SprueEditor::BasePropertyEditor* editor, std::shared_ptr<DataSource>)
    {

    }

    void UrhoVec2PropertyHandler::ShowEditValue(SprueEditor::BasePropertyEditor* aEditor, std::shared_ptr<DataSource> dataSource)
    {
        Vector2PropertyEditor* editor = (Vector2PropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(dataSource))
            if (auto editing_ = data->GetObject())
            {
                if (auto prop = Attribute(editing_.Get()))
                {
                    Urho3D::Variant curVal;
                    prop->accessor_->Get(editing_.Get(), curVal);
                    editor->xSpin_->setValue(curVal.GetVector2().x_);
                    editor->ySpin_->setValue(curVal.GetVector2().y_);
                }
            }
    }

    void UrhoVec2PropertyHandler::CommitEditValue(SprueEditor::BasePropertyEditor* aEditor, std::shared_ptr<DataSource> dataSource)
    {
        Vector2PropertyEditor* editor = (Vector2PropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(dataSource))
            if (auto editing = data->GetObject())
            {
                if (auto property = Attribute(editing.Get()))
                {
                    Urho3D::Vector2 value;
                    value.x_ = editor->xSpin_->value();
                    value.y_ = editor->ySpin_->value();
                    URHO_PROPERTY_UNDO(data, hash_.Value(), value);
                }
            }
    }

    //// Vector3
    void UrhoVec3PropertyHandler::ConfigureEditor(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        
    }
    void UrhoVec3PropertyHandler::ShowEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        Vector3PropertyEditor* editor = (Vector3PropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(datasource))
            if (auto editing_ = data->GetObject())
            {
                if (auto prop = Attribute(editing_.Get()))
                {
                    Urho3D::Variant curVal;
                    prop->accessor_->Get(editing_.Get(), curVal);
                    editor->xSpin_->setValue(curVal.GetVector3().x_);
                    editor->xSpin_->setValue(curVal.GetVector3().y_);
                    editor->xSpin_->setValue(curVal.GetVector3().z_);
                }
            }
    }
    void UrhoVec3PropertyHandler::CommitEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        Vector3PropertyEditor* editor = (Vector3PropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(datasource))
            if (auto editing = data->GetObject())
            {
                if (auto property = Attribute(editing.Get()))
                {
                    Urho3D::Vector3 value;
                    value.x_ = editor->xSpin_->value();
                    value.y_ = editor->ySpin_->value();
                    value.z_ = editor->zSpin_->value();
                    //SprueEditor::SprueCommand* cmd = new UrhoEditor::UrhoPropertyCmd(data, hash_.Value(), value, data->GetObject()->GetAttribute(name_));
                    //SprueEditor::SprueKitEditor::GetInstance()->GetDocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd);
                    URHO_PROPERTY_UNDO(data, hash_.Value(), value);
                }
            }
    }
    
    // Quaternion
    void UrhoQuatPropertyHandler::ConfigureEditor(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        
    }
    void UrhoQuatPropertyHandler::ShowEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        QuatPropertyEditor* editor = (QuatPropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(datasource))
            if (auto editing_ = data->GetObject())
            {
                if (auto prop = Attribute(editing_.Get()))
                {
                    Urho3D::Variant curVal;
                    prop->accessor_->Get(editing_.Get(), curVal);
                    Urho3D::Vector3 asVec = curVal.GetQuaternion().EulerAngles();
                    editor->xSpin_->setValue(asVec.x_);
                    editor->xSpin_->setValue(asVec.y_);
                    editor->xSpin_->setValue(asVec.z_);
                }
            }
    }
    void UrhoQuatPropertyHandler::CommitEditValue(BasePropertyEditor* aEditor, std::shared_ptr<DataSource> datasource)
    {
        Vector3PropertyEditor* editor = (Vector3PropertyEditor*)aEditor;
        if (auto data = std::dynamic_pointer_cast<SerializableDataSource>(datasource))
            if (auto editing = data->GetObject())
            {
                if (auto property = Attribute(editing.Get()))
                {
                    Urho3D::Quaternion value;
                    value.FromEulerAngles(editor->xSpin_->value(), editor->ySpin_->value(), editor->zSpin_->value());
                    //SprueEditor::SprueCommand* cmd = new UrhoEditor::UrhoPropertyCmd(data, hash_.Value(), value, data->GetObject()->GetAttribute(name_));
                    //SprueEditor::SprueKitEditor::GetInstance()->GetDocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd);
                    URHO_PROPERTY_UNDO(data, hash_.Value(), value);
                }
            }
    }
}