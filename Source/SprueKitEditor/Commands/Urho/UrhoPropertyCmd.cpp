#include "UrhoPropertyCmd.h"

#include <Urho3D/Core/Context.h>

#include <SprueEngine/FString.h>

namespace UrhoEditor
{

    UrhoPropertyCmd::UrhoPropertyCmd(std::shared_ptr<SerializableDataSource> object, unsigned property, Urho3D::Variant newValue, Urho3D::Variant oldValue) :
        object_(object),
        newValue_(newValue),
        oldValue_(oldValue),
        property_(property)
    {
        auto attr = object->GetObject()->GetContext()->GetAttributes(object->GetObject()->GetType());
        if (attr->Size() > property)
            SetText(QString("Set '%1' to '%2'").arg((*attr)[property].name_.CString(), newValue.ToString().CString()));
    }

    UrhoPropertyCmd::~UrhoPropertyCmd()
    {

    }

    void UrhoPropertyCmd::Redo()
    {
        object_->GetObject()->SetAttribute(property_, newValue_);
        if (!IsFirstRedo())
            SprueEditor::SprueKitEditor::GetInstance()->GetObjectSelectron()->NotifyDataChangedSoft(0x0, object_);
        SimpleSmartCommand::Redo();
    }

    void UrhoPropertyCmd::Undo()
    {
        object_->GetObject()->SetAttribute(property_, oldValue_);
        SprueEditor::SprueKitEditor::GetInstance()->GetObjectSelectron()->NotifyDataChangedSoft(0x0, object_);
        SimpleSmartCommand::Undo();
    }

    bool UrhoPropertyCmd::CanMergeWith(const SmartCommand* other)
    {
        if (const UrhoPropertyCmd* rhs = dynamic_cast<const UrhoPropertyCmd*>(other))
        {
            if (rhs->object_->Equals(object_.get()) && rhs->property_ == property_)
                return true;
        }
        return false;
    }

    bool UrhoPropertyCmd::MergeWith(const SmartCommand* other)
    {
        UrhoPropertyCmd* rhs = (UrhoPropertyCmd*)other;
        if (rhs && rhs->object_->Equals(object_.get()) && rhs->property_ == property_)
        {
            newValue_ = rhs->newValue_;
            SetText(rhs->GetText());
            return true;
        }
        return false;
    }

    bool UrhoPropertyCmd::ShouldBounce()
    {
        return oldValue_ == newValue_;
    }

    void UrhoPropertyCmd::MadeCurrent()
    {
        SprueEditor::SprueKitEditor::GetInstance()->GetObjectSelectron()->NotifyDataChanged(0x0, object_, -1);
    }

    UrhoGizmoCmd::UrhoGizmoCmd(std::shared_ptr<NodeDataSource> object, const Urho3D::Matrix3x4& oldMatrix, const Urho3D::Matrix3x4& newMatrix) :
        object_(object), old_(oldMatrix), new_(newMatrix)
    {
    }

    void UrhoGizmoCmd::Redo()
    {
        if (!IsFirstRedo())
        {
            if (object_)
            {
                if (auto object = object_->GetNode())
                {
                    Urho3D::Vector3 pos; Urho3D::Quaternion rot; Urho3D::Vector3 scl;
                    new_.Decompose(pos, rot, scl);
                    object->SetWorldPosition(pos);
                    object->SetWorldRotation(rot);
                    object->SetWorldScale(scl);
                    SprueEditor::SprueKitEditor::GetInstance()->GetObjectSelectron()->NotifyDataChangedSoft(0x0, object_);
                }
            }
        }
        SimpleSmartCommand::Redo();
    }

    void UrhoGizmoCmd::Undo()
    {
        if (object_)
        {
            if (auto object = object_->GetNode())
            {
                Urho3D::Vector3 pos; Urho3D::Quaternion rot; Urho3D::Vector3 scl;
                old_.Decompose(pos, rot, scl);
                object->SetWorldPosition(pos);
                object->SetWorldRotation(rot);
                object->SetWorldScale(scl);
                SprueEditor::SprueKitEditor::GetInstance()->GetObjectSelectron()->NotifyDataChangedSoft(0x0, object_);
            }
        }
        SimpleSmartCommand::Undo();
    }

    void UrhoGizmoCmd::MadeCurrent()
    {
        SprueEditor::SprueKitEditor::GetInstance()->GetObjectSelectron()->NotifyDataChanged(0x0, object_);
    }

    bool UrhoGizmoCmd::CanMergeWith(const SmartCommand* other)
    {
        if (const UrhoGizmoCmd* rhs = dynamic_cast<const UrhoGizmoCmd*>(other))
        {
            if (rhs->object_ && object_ && rhs->object_->GetNode() == object_->GetNode())
                return true;
        }
        return false;
    }

    bool UrhoGizmoCmd::MergeWith(const SmartCommand* other)
    {
        const UrhoGizmoCmd* rhs = (const UrhoGizmoCmd*)other;
        if (rhs)
        {
            if (rhs->object_ && object_ && rhs->object_->GetNode() == object_->GetNode())
            {
                new_ = rhs->new_;
                return true;
            }
        }
        return false;
    }

    bool UrhoGizmoCmd::ShouldBounce()
    {
        return old_.Equals(new_);
    }
}