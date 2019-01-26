#include "PerspectiveView.h"

#include "InputRecorder.h"
#include "../SprueKitEditor.h"
#include "../Panels/SceneTreePanel.h"
#include "../UrhoHelpers.h"
#include "ViewUtility.h"

#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include <SprueEngine/FString.h>

#include <set>

using namespace Urho3D;

namespace SprueEditor
{

PerspectiveView::PerspectiveView(Urho3D::Context* context, DocumentBase* document) :
    ViewBase(context, document)
{
    scene_ = new Scene(GetContext());
    scene_->SetVar("TestVariable", 52);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    // Create camera.
    cameraNode_ = scene_->CreateChild("Camera");
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFov(45);

    ResourceCache* cache = scene_->GetSubsystem<ResourceCache>();

    // Create the node for holding billboard components for gizmo points
    gizmoPointsNode_ = scene_->CreateChild("SECRET_GIZMO_POINTS");
    // Gizmo control points
    BillboardSet* controlPointBBSet = gizmoPointsNode_->CreateComponent<BillboardSet>();
    controlPointBBSet->SetSorted(true);
    controlPointBBSet->SetFixedScreenSize(true);
    controlPointBBSet->SetNumBillboards(2048);
    controlPointBBSet->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Editor/GizmoControlPoint.xml"));
    // Gizmo deform points
    //??gizmoPointsNode_->CreateComponent<BillboardSet>();

    //camera->SetOrthographic(true);
    Graphics* graphic = GetSubsystem<Graphics>();
    //camera->SetOrthoSize(graphic->GetHeight());
    cameraNode_->SetPosition(Vector3(0, 5, -15));
    cameraNode_->LookAt(Vector3(0, 0, 0));
    Quaternion camQuat = cameraNode_->GetWorldRotation();
    cameraPitch_ = camQuat.PitchAngle();
    cameraYaw_ = camQuat.YawAngle();

    viewport_ = new Viewport(context_, scene_, camera);

    ::SprueEditor::CreateGrid(scene_, 32, Vector3(1, 1, 1), false);
}

PerspectiveView::~PerspectiveView()
{
    if (frameStats_)
        frameStats_->Remove(); // Urho3D will delete it
    if (camPos_)
        camPos_->Remove(); // Urho3D will delete it
    if (triStats_)
        triStats_->Remove(); // Urho3D will delete it

    triStats_ = camPos_ = frameStats_ = 0x0;
}

void PerspectiveView::Activate()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    renderer->SetViewport(0, viewport_);

    if (frameStats_ == 0x0)
        frameStats_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
    if (camPos_ == 0x0)
        camPos_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
    if (triStats_ == 0x0)
        triStats_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();

    frameStats_->SetVisible(true);
    frameStats_->SetText("TEST");

    frameStats_->SetFont(context_->GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    frameStats_->SetColor(Color(1, 1, 0));
    frameStats_->SetAlignment(Urho3D::HorizontalAlignment::HA_LEFT, Urho3D::VerticalAlignment::VA_TOP);

    camPos_->SetVisible(true);
    camPos_->SetFont(context_->GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    camPos_->SetColor(Color(1, 1, 0));
    camPos_->SetAlignment(Urho3D::HorizontalAlignment::HA_LEFT, Urho3D::VerticalAlignment::VA_BOTTOM);

    triStats_->SetVisible(true);
    triStats_->SetAlignment(Urho3D::HorizontalAlignment::HA_RIGHT, Urho3D::VerticalAlignment::VA_TOP);
    triStats_->SetFont(context_->GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    triStats_->SetColor(Color(1, 1, 0));
}

void PerspectiveView::Deactivate()
{
    ViewBase::Deactivate();
    if (triStats_)
        triStats_->SetVisible(false);
    if (frameStats_)
        frameStats_->SetVisible(false);
    if (camPos_)
        camPos_->SetVisible(false);
}

void PerspectiveView::RenderUpdate()
{
    if (frameStats_)
        frameStats_->SetText(FString("FPS: %1", (1.0f / context_->GetSubsystem<Renderer>()->GetFrameInfo().timeStep_)).c_str());
    if (camPos_)
        camPos_->SetText(FString("X: %1 Y: %2 Z: %3", cameraNode_->GetWorldPosition().x_, cameraNode_->GetWorldPosition().y_, cameraNode_->GetWorldPosition().z_).c_str());
    if (triStats_)
        triStats_->SetText(FString("Triangles: %1", GetTriangleCount()).c_str());

    ViewBase::RenderUpdate();
}

void PerspectiveView::GizmoUpdated(std::shared_ptr<Gizmo> giz)
{
    for (auto g : gizmos_)
    {
        if (g != giz)
            g->RefreshValue();
    }
}

void PerspectiveView::SetGizmoMode(SprueEditor::GizmoMode mode)
{
    for (auto giz : gizmos_)
    {
        if (mode == SprueEditor::GM_Translate && giz->CanTranslate())
            giz->SetForTranslation();
        else if (mode == SprueEditor::GM_Rotate && giz->CanRotate())
            giz->SetForRotation();
        else if (mode == SprueEditor::GM_Scale && giz->CanScale())
            giz->SetForScale();
        else if (mode == SprueEditor::GM_Axial && giz->CanRotate())
            giz->SetForAxial();
        else
            giz->SetForNone();
    }
}

void PerspectiveView::SetGizmoLocal(bool isLocal)
{
    for (auto giz : gizmos_)
        giz->SetLocal(isLocal);
}

unsigned PerspectiveView::GetTriangleCount() const
{
    return context_->GetSubsystem<Renderer>()->GetNumPrimitives();
}

}