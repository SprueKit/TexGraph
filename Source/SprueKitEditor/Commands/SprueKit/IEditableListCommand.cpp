#include "IEditableListCommand.h"

#include <SprueEngine/FString.h>

using namespace SprueEngine;

namespace SprueEditor
{

// Add
    IEditableListAddCommand::IEditableListAddCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& object, unsigned index) :
        editable_(editable),
        property_(property),
        object_(object),
        index_(index)
    {
        SetText(FString("Add %1 to %2", ((IEditable*)object.getVoidPtr())->GetTypeName(), property->GetName()).c_str());
    }

    IEditableListAddCommand::~IEditableListAddCommand()
    {
        // if we have undone then delete it
        if (lastActionWasUndo_)
        {
            if (IEditable* editable = (IEditable*)object_.getVoidPtr())
                delete editable;
        }
    }

    void IEditableListAddCommand::Redo()
    {
        VariantVector list = property_->Get(editable_->GetEditable()).getVariantVector();
        list.push_back(object_);
        property_->Set(editable_->GetEditable(), list);
        SimpleSmartCommand::Redo();
    }

    void IEditableListAddCommand::Undo()
    {
        VariantVector list = property_->Get(editable_->GetEditable()).getVariantVector();
        list.erase(list.begin() + index_);
        property_->Set(editable_->GetEditable(), list);
        SimpleSmartCommand::Undo();
    }

// Delete
    IEditableListDeleteCommand::IEditableListDeleteCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& object, unsigned index) :
        editable_(editable),
        property_(property),
        object_(object),
        index_(index)
    {
        SetText(FString("Delete %1 from %2", ((IEditable*)object.getVoidPtr())->GetTypeName(), property->GetName()).c_str());
    }

    IEditableListDeleteCommand::~IEditableListDeleteCommand()
    {
        // if we haven't undone then delete it
        if (!lastActionWasUndo_)
        {
            if (IEditable* editable = (IEditable*)object_.getVoidPtr())
                delete editable;
        }
    }

    void IEditableListDeleteCommand::Redo()
    {
        VariantVector list = property_->Get(editable_->GetEditable()).getVariantVector();
        list.erase(list.begin() + index_);
        property_->Set(editable_->GetEditable(), list);
        SimpleSmartCommand::Redo();
    }

    void IEditableListDeleteCommand::Undo()
    {
        VariantVector list = property_->Get(editable_->GetEditable()).getVariantVector();
        list.insert(list.begin() + index_, object_);
        property_->Set(editable_->GetEditable(), list);
        SimpleSmartCommand::Undo();
    }

// Move

    IEditableListMoveCommand::IEditableListMoveCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& object, unsigned oldIndex, unsigned newIndex) :
        editable_(editable),
        property_(property),
        object_(object),
        oldIndex_(oldIndex),
        newIndex_(newIndex)
    {
        SetText(FString("Move %1 in %2", ((IEditable*)object.getVoidPtr())->GetTypeName(), property->GetName()).c_str());
    }

    void IEditableListMoveCommand::Redo()
    {
        VariantVector list = property_->Get(editable_->GetEditable()).getVariantVector();
        list.erase(list.begin() + oldIndex_);
        list.insert(list.begin() + newIndex_, object_);
        property_->Set(editable_->GetEditable(), list);
        SimpleSmartCommand::Redo();
    }

    void IEditableListMoveCommand::Undo()
    {
        VariantVector list = property_->Get(editable_->GetEditable()).getVariantVector();
        list.erase(list.begin() + newIndex_);
        list.insert(list.begin() + oldIndex_, object_);
        property_->Set(editable_->GetEditable(), list);
        SimpleSmartCommand::Undo();
    }

    bool IEditableListMoveCommand::CanMergeWith(const SmartCommand* other)
    {
        if (const IEditableListMoveCommand* rhs = dynamic_cast<const IEditableListMoveCommand*>(other))
        {
            if (object_.getVoidPtr() == rhs->object_.getVoidPtr())
                return true;
        }
        return false;
    }

    bool IEditableListMoveCommand::MergeWith(const SmartCommand* other)
    {
        if (const IEditableListMoveCommand* rhs = dynamic_cast<const IEditableListMoveCommand*>(other))
        {
            if (object_.getVoidPtr() == rhs->object_.getVoidPtr())
            {
                newIndex_ = rhs->newIndex_;
                SetText(rhs->GetText());
                return true;
            }
        }
        return false;
    }
}