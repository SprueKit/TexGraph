#pragma once

#include "../../Data/SprueDataSources.h"

#include "../CommandID.h"
#include "../../SprueKitEditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Commands/SmartUndoStack.h>

#include <SprueEngine/StringHash.h>
#include <SprueEngine/Variant.h>

namespace SprueEngine
{
    class IEditable;
    class TypeProperty;
}

namespace SprueEditor
{

    class IEditablePropertyCommand : public SimpleSmartCommand
    {
    public:
        IEditablePropertyCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& oldValue, const SprueEngine::Variant& newValue, const SprueEngine::StringHash& reportAsHash);
        virtual ~IEditablePropertyCommand();

        virtual void Redo() override;
        virtual void Undo() override;
        virtual bool CanMergeWith(const SmartCommand* rhs) override;
        virtual bool MergeWith(const SmartCommand* other) override;
        virtual bool ShouldBounce() override;
        virtual void MadeCurrent() override;
        virtual int GetID() const override { return CMD_IEDITABLE_PROPERTY; }

        std::shared_ptr<IEditableDataSource> editable_;
        SprueEngine::TypeProperty* property_;
        SprueEngine::Variant oldValue_;
        SprueEngine::Variant newValue_;
        SprueEngine::StringHash reportAsHash_;
    };

    class IEditableSelectableChildObjectCommand : public IEditablePropertyCommand
    {
    public:
        IEditableSelectableChildObjectCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& oldValue, const SprueEngine::Variant& newValue);
        virtual ~IEditableSelectableChildObjectCommand();

        virtual void Redo() override;
        virtual void Undo() override;
        virtual bool CanMergeWith(const SmartCommand* rhs) override;
        virtual bool MergeWith(const SmartCommand* other) override;
        virtual bool ShouldBounce() override { return false; }
        virtual int GetID() const override { return CMD_IEDITABLE_CHILD_OBJECT; }

    private:
        bool lastActionWasUndo_;
    };

#define SPRUE_PROPERTY_UNDO(VALUE) { SprueCommand* cmd = new IEditablePropertyCommand(editing_, property_, property_->Get(editing_->GetEditable()), VALUE, reportAsHash_.value_ != 0 ? reportAsHash_ : property_->GetHash()); \
    Global_DocumentManager->GetActiveDocument()->GetUndoStack()->push(cmd); }

}