#pragma once

#include "ViewManager.h"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/Text.h>

namespace SprueEditor
{

/// Base class for a view that has a perspective camera (Design/Animate/Sculpt/Paint)
/// Primarily handles viewport control
class PerspectiveView : public ViewBase
{
public:
    PerspectiveView(Urho3D::Context* context, DocumentBase* document);
    virtual ~PerspectiveView();
    virtual void Activate() override;
    virtual void Deactivate() override;
    virtual void GizmoUpdated(std::shared_ptr<Gizmo> giz) override;
    virtual void SetGizmoMode(SprueEditor::GizmoMode mode) override;
    virtual void SetGizmoLocal(bool isLocal) override;
    virtual void RenderUpdate() override;

    virtual unsigned GetTriangleCount() const;

protected:
    Urho3D::SharedPtr<Urho3D::Text> frameStats_;
    Urho3D::SharedPtr<Urho3D::Text> camPos_;
    Urho3D::SharedPtr<Urho3D::Text> triStats_;

    /// Angle of camera pitch
    float cameraPitch_;
    /// Angle of camera yaw
    float cameraYaw_;
    /// Node that contains the BillboardSet for the gizmo points indicator (named "SECRET_GIZMO_POINTS")
    Urho3D::SharedPtr<Urho3D::Node> gizmoPointsNode_;
};

}