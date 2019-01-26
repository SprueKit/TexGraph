#pragma once

#include "../../Data/UrhoDataSources.h"

#include "../CommandID.h"

#include <EditorLib/Commands/SmartUndoStack.h>

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Component.h>

#include <memory>

namespace UrhoEditor
{

    namespace SceneCommands
    {

        class AddComponentCmd : public SimpleSmartCommand
        {
        public:
            AddComponentCmd(Urho3D::Node* node, std::shared_ptr<SerializableDataSource> comp);

            virtual void Undo() override;
            virtual void Redo() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }
            virtual int GetID() const override { return CMD_URHO_ADD_COMP; }
            virtual void MadeCurrent() override;

            Urho3D::SharedPtr<Urho3D::Node> node_;
            std::shared_ptr<SerializableDataSource> comp_;
        };

        class DeleteComponentCmd : public SimpleSmartCommand
        {
        public:
            DeleteComponentCmd(Urho3D::Node* node, std::shared_ptr<SerializableDataSource> comp);

            virtual void Undo() override;
            virtual void Redo() override;
            virtual int GetID() const override { return CMD_URHO_DELETE_COMP; } 
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }
            virtual void MadeCurrent() override;

            Urho3D::SharedPtr<Urho3D::Node> node_;
            std::shared_ptr<SerializableDataSource> comp_;
        };

        class MoveComponentOwnerCmd : public SimpleSmartCommand
        {
        public:
            MoveComponentOwnerCmd(Urho3D::Node* oldParent, Urho3D::Node* newParent, std::shared_ptr<SerializableDataSource> comp);
            virtual void Undo() override;
            virtual void Redo() override;
            virtual int GetID() const override { return CMD_URHO_MOVE_COMP; }
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }
            virtual void MadeCurrent() override;

            Urho3D::SharedPtr<Urho3D::Node> oldNode_, newNode_;
            std::shared_ptr<SerializableDataSource> comp_;
            unsigned oldIndex_, newIndex_;
        };


        class AddNodeCmd : public SimpleSmartCommand
        {
        public:
            AddNodeCmd(Urho3D::Node* parent, std::shared_ptr<SerializableDataSource> child);

            virtual void Undo() override;
            virtual void Redo() override;
            virtual int GetID() const override { return CMD_URHO_ADD_NODE; }
            virtual void MadeCurrent() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            Urho3D::SharedPtr<Urho3D::Node> parent_;
            std::shared_ptr<SerializableDataSource> self_;
        };

        class AddUIElementCmd : public SimpleSmartCommand
        {
        public:
            AddUIElementCmd(Urho3D::UIElement* parent, std::shared_ptr<SerializableDataSource> child);

            virtual void Undo() override;
            virtual void Redo() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }
            virtual int GetID() const override { return CMD_URHO_ADD_UIELEMENT; }
            virtual void MadeCurrent() override;

            Urho3D::SharedPtr<Urho3D::UIElement> parent_;
            std::shared_ptr<SerializableDataSource> self_;
        };

        class DeleteNodeCmd : public SimpleSmartCommand
        {
        public:
            DeleteNodeCmd(Urho3D::Node* parent, std::shared_ptr<SerializableDataSource> self);
            virtual void Undo() override;
            virtual void Redo() override;
            virtual int GetID() const override { return CMD_URHO_DELETE_NODE; }
            virtual void MadeCurrent() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            Urho3D::SharedPtr<Urho3D::Node> parent_;
            std::shared_ptr<SerializableDataSource> self_;
            unsigned index_;
        };

        class DeleteUIElementCmd : public SimpleSmartCommand
        {
        public:
            DeleteUIElementCmd(Urho3D::UIElement* parent, std::shared_ptr<SerializableDataSource> self);
            virtual void Undo() override;
            virtual void Redo() override;
            virtual int GetID() const override { return CMD_URHO_DELETE_UIELEMENT; }
            virtual void MadeCurrent() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            Urho3D::SharedPtr<Urho3D::UIElement> parent_;
            std::shared_ptr<SerializableDataSource> self_;
            unsigned index_;
        };

        class MoveNodeCmd : public SimpleSmartCommand
        {
        public:
            MoveNodeCmd(Urho3D::Node* oldParent, Urho3D::Node* newParent, std::shared_ptr<SerializableDataSource> self);
            MoveNodeCmd(Urho3D::Node* oldParent, Urho3D::Node* newParent, std::shared_ptr<SerializableDataSource> self, int newIndex);
            virtual void Undo() override;
            virtual void Redo() override;
            virtual int GetID() const override { return CMD_URHO_MOVE_NODE; }
            virtual void MadeCurrent() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            Urho3D::SharedPtr<Urho3D::Node> oldParent_, newParent_;
            std::shared_ptr<SerializableDataSource> self_;
            unsigned oldIndex_;
            unsigned newIndex_ = UINT_MAX;
        };

        class MoveUIElementCmd : public SimpleSmartCommand
        {
        public:
            MoveUIElementCmd(Urho3D::UIElement* oldParent, Urho3D::UIElement* newParent, std::shared_ptr<SerializableDataSource> self);
            MoveUIElementCmd(Urho3D::UIElement* oldParent, Urho3D::UIElement* newParent, std::shared_ptr<SerializableDataSource> self, int newIndex);
            virtual void Undo() override;
            virtual void Redo() override;
            virtual int GetID() const override { return CMD_URHO_MOVE_UIELEMENT; }
            virtual void MadeCurrent() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            Urho3D::SharedPtr<Urho3D::UIElement> oldParent_, newParent_;
            std::shared_ptr<SerializableDataSource> self_;
            unsigned oldIndex_;
            unsigned newIndex_ = UINT_MAX;
        };
    }
}