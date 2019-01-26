#pragma once

#include "../../Data/UrhoDataSources.h"

#include <QUndoCommand>
#include <QUndoStack>

#include "../CommandID.h"
#include "../../SprueKitEditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Commands/SmartUndoStack.h>

#include <Urho3D/Scene/Serializable.h>

namespace UrhoEditor
{

    class UrhoPropertyCmd : public SimpleSmartCommand
    {
    public:
        UrhoPropertyCmd(std::shared_ptr<SerializableDataSource> object, unsigned property, Urho3D::Variant newvalue, Urho3D::Variant oldValue);
        virtual ~UrhoPropertyCmd();

        virtual void Redo() override;
        virtual void Undo() override;

        virtual bool CanMergeWith(const SmartCommand* rhs) override;
        virtual bool MergeWith(const SmartCommand* other) override;
        virtual bool ShouldBounce() override;
        virtual void MadeCurrent() override;
        virtual int GetID() const override { return CMD_URHO_PROPERTY; }

        std::shared_ptr<SerializableDataSource> object_;
        Urho3D::Variant newValue_, oldValue_;
        unsigned property_;
    };

    class UrhoGizmoCmd : public SimpleSmartCommand
    {
    public:
        UrhoGizmoCmd(std::shared_ptr<NodeDataSource> object, const Urho3D::Matrix3x4& oldMatrix, const Urho3D::Matrix3x4& newMatrix);
        
        virtual void Redo() override;
        virtual void Undo() override;
        virtual void MadeCurrent() override;

        virtual bool CanMergeWith(const SmartCommand* rhs) override;
        virtual bool MergeWith(const SmartCommand* other) override;
        virtual bool ShouldBounce() override;
        virtual int GetID() const override { return CMD_URHO_GIZMO; }
        
        std::shared_ptr<NodeDataSource> object_;
        Urho3D::Matrix3x4 old_;
        Urho3D::Matrix3x4 new_;
    };

#define URHO_PROPERTY_UNDO(EDITING, PROPERTY, NEWVALUE) { SmartCommand* cmd = new UrhoEditor::UrhoPropertyCmd(EDITING, PROPERTY, NEWVALUE, EDITING->GetObject()->GetAttribute(PROPERTY)); \
    Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd); }
}