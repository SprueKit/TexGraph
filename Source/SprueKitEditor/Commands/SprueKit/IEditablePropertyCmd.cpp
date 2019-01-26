#include "IEditablePropertyCmd.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/IEditable.h>

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"

#include <QString>

#include <algorithm>

using namespace SprueEngine;

namespace SprueEditor
{

    IEditablePropertyCommand::IEditablePropertyCommand(std::shared_ptr<IEditableDataSource> editable, TypeProperty* property, const Variant& oldValue, const Variant& newValue, const StringHash& reportAsHash) :
        editable_(editable),
        property_(property),
        oldValue_(oldValue),
        newValue_(newValue),
        reportAsHash_(reportAsHash)
    {
        SetText(QString("Set '%1' to '%2'").arg(property->GetName().c_str(), newValue.ConvertToString().c_str()));
    }

    IEditablePropertyCommand::~IEditablePropertyCommand()
    {
    }

    void IEditablePropertyCommand::Redo()
    {
        if (editable_)
        {
            property_->Set(editable_->GetEditable(), newValue_);
            if (!IsFirstRedo())
                Global_ObjectSectron()->NotifyDataChangedSoft(0x0, editable_, reportAsHash_ != StringHash() ? reportAsHash_ : property_->GetHash());
        }
        SimpleSmartCommand::Redo();
    }

    void IEditablePropertyCommand::Undo()
    {
        if (editable_)
        {
            property_->Set(editable_->GetEditable(), oldValue_);
            Global_ObjectSectron()->NotifyDataChangedSoft(0x0, editable_, reportAsHash_ != StringHash() ? reportAsHash_ : property_->GetHash());
        }
        SimpleSmartCommand::Undo();
    }

    bool IEditablePropertyCommand::CanMergeWith(const SmartCommand* other)
    {
        if (const IEditablePropertyCommand* rhs = dynamic_cast<const IEditablePropertyCommand*>(other))
        {
            if (rhs->editable_->Equals(editable_.get()) && rhs->property_ == property_)
                return true;
        }
        return false;
    }

    bool IEditablePropertyCommand::MergeWith(const SmartCommand* other)
    {
        IEditablePropertyCommand* rhs = (IEditablePropertyCommand*)other;
        if (rhs && rhs->editable_->Equals(editable_.get()) && rhs->property_ == property_)
        {
            newValue_ = rhs->newValue_;
            SetText(rhs->GetText());
            return true;
        }
        return false;
    }

    bool IEditablePropertyCommand::ShouldBounce()
    {
        return newValue_ == oldValue_;
    }

    void IEditablePropertyCommand::MadeCurrent()
    {
        Global_ObjectSectron()->NotifyDataChanged(0x0, editable_->GetParent() ? editable_->GetParent() : editable_, reportAsHash_ != StringHash() ? reportAsHash_ : property_->GetHash());
    }

    IEditableSelectableChildObjectCommand::IEditableSelectableChildObjectCommand(std::shared_ptr<IEditableDataSource> editable, TypeProperty* property, const Variant& oldValue, const Variant& newValue) :
        IEditablePropertyCommand(editable, property, oldValue, newValue, StringHash())
    {
        IEditable* newEditable = (IEditable*)newValue.getVoidPtr();
        SetText(QString("Set '%1' to '%2'").arg(property->GetName().c_str(), newEditable->GetTypeName()));
    }

    IEditableSelectableChildObjectCommand::~IEditableSelectableChildObjectCommand()
    {
        if (lastActionWasUndo_)
            if (IEditable* newEditable = ((IEditable*)newValue_.getVoidPtr()))
                delete newEditable;
        else 
            if (IEditable* oldEditable = ((IEditable*)oldValue_.getVoidPtr()))
                delete oldEditable;
    }

    void IEditableSelectableChildObjectCommand::Redo()
    {
        IEditablePropertyCommand::Redo();
    }

    void IEditableSelectableChildObjectCommand::Undo()
    {
        IEditablePropertyCommand::Undo();
    }

    bool IEditableSelectableChildObjectCommand::CanMergeWith(const SmartCommand* other)
    {
        if (const IEditableSelectableChildObjectCommand* rhs = dynamic_cast<const IEditableSelectableChildObjectCommand*>(other))
        {
            return true;
        }
        return false;
    }

    bool IEditableSelectableChildObjectCommand::MergeWith(const SmartCommand* other)
    {
        if (const IEditableSelectableChildObjectCommand* rhs = dynamic_cast<const IEditableSelectableChildObjectCommand*>(other))
        {
            delete ((IEditable*)newValue_.getVoidPtr());
            newValue_ = rhs->newValue_;
            IEditable* newEditable = (IEditable*)newValue_.getVoidPtr();
            SetText(QString("Set '%1' to '%2'").arg(property_->GetName().c_str(), newEditable->GetTypeName()));
            return true;
        }
        return false;
    }
}