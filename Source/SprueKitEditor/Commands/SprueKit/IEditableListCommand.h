#pragma once

#include "../../Data/SprueDataSources.h"
#include "../CommandID.h"

#include <EditorLib/Commands/SmartUndoStack.h>

#include <SprueEngine/IEditable.h>
#include <SprueEngine/Property.h>

namespace SprueEditor
{

    class IEditableListAddCommand : public SimpleSmartCommand
    {
    public:
        IEditableListAddCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& object, unsigned index);
        virtual ~IEditableListAddCommand();

        virtual void Redo() override;
        virtual void Undo() override;
        virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }
        virtual bool MergeWith(const SmartCommand* other) override { return false; }
        virtual int GetID() const override { return CMD_IEDITABLE_CHILD_LIST_ADD; }

    private:
        std::shared_ptr<IEditableDataSource> editable_;
        SprueEngine::TypeProperty* property_;
        SprueEngine::Variant object_;
        unsigned index_;
    };

    class IEditableListDeleteCommand : public SimpleSmartCommand
    {
    public:
        IEditableListDeleteCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& object, unsigned index);
        virtual ~IEditableListDeleteCommand();

        virtual void Redo() override;
        virtual void Undo() override;
        virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }
        virtual bool MergeWith(const SmartCommand* other) override { return false; }
        virtual int GetID() const override { return CMD_IEDITABLE_CHILD_LIST_DELETE; }

    private:
        std::shared_ptr<IEditableDataSource> editable_;
        SprueEngine::TypeProperty* property_;
        SprueEngine::Variant object_;
        unsigned index_;
    };

    class IEditableListMoveCommand : public SimpleSmartCommand
    {
    public:
        IEditableListMoveCommand(std::shared_ptr<IEditableDataSource> editable, SprueEngine::TypeProperty* property, const SprueEngine::Variant& object, unsigned oldIndex, unsigned newIndex);
        
        virtual void Redo() override;
        virtual void Undo() override;
        virtual bool CanMergeWith(const SmartCommand* rhs) override;
        virtual bool MergeWith(const SmartCommand* other) override;
        virtual int GetID() const override { return CMD_IEDITABLE_CHILD_LIST_MOVE; }

    private:
        std::shared_ptr<IEditableDataSource> editable_;
        SprueEngine::TypeProperty* property_;
        SprueEngine::Variant object_;
        unsigned oldIndex_;
        unsigned newIndex_;
    };
}