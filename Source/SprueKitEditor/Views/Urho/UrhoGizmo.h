#pragma once

#include "../Gizmo.h"
#include "../../Data/UrhoDataSources.h"

#include <EditorLib/Commands/SmartUndoStack.h>

namespace UrhoEditor
{

    /// Implementation of the editor gizmo for use with Urho3D objects (instead of SprueEngine::Gizmo objects)
    class UrhoGizmo : public SprueEditor::Gizmo
    {
    public:
        /// Constructs with the node that will be the gizmo and the 'transformNode' that will be manipulated.
        UrhoGizmo(Urho3D::Node* transformNode);
        /// Destruct.
        virtual ~UrhoGizmo();

        /// Gets the object that this gizmo is manipulating.
        virtual std::shared_ptr<DataSource> GetEditObject() override { return source_; }

        /// Initialize the gizmo (necessary for working arount ctor's and virtual methods)
        virtual void Construct() override;

        /// Return the transform.
        virtual Urho3D::Matrix3x4 GetTransform() override;
        
        /// Apply a changed transform to the edit object.
        virtual void ApplyTransform(const Urho3D::Matrix3x4& transform) override;

        /// Update the gizmo from its source object.
        virtual void RefreshValue() override;
        virtual bool Equal(Gizmo* rhs) const override;

        virtual void PushUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) override;
        virtual SmartCommand* CreateUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) override;

    protected:
        /// The Urho3D::Node that is being transformed by this gizmo
        std::shared_ptr<UrhoEditor::NodeDataSource> source_;
        Urho3D::Matrix3x4 originalTransform_;
        Urho3D::Node* transformingNode_ = 0x0;
    };

}