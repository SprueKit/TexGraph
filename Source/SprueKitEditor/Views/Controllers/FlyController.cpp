#include "FlyController.h"

#include "../InputRecorder.h"
#include "../ViewBase.h"

#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

using namespace Urho3D;

namespace SprueEditor
{

FlyController::FlyController(Urho3D::Node* cameraNode, ViewManager* manager, ViewBase* view, InputRecorder* input) :
    ViewController(manager, view, input),
    cameraNode_(cameraNode),
    cameraPitch_(0.0f),
    cameraYaw_(0.0f)
{
    Quaternion camQuat = cameraNode_->GetWorldRotation();
    cameraPitch_ = camQuat.PitchAngle();
    cameraYaw_ = camQuat.YawAngle();
}

void FlyController::PassiveChecks(const ViewController* currentActive)
{
    const bool shiftHeld = input_->IsShiftDown();
    const float multiplier = shiftHeld ? 5.0f : 1.0f;

    auto camera = cameraNode_->GetComponent<Camera>();
    if (input_->GetWheel())
    {
        const float scale = input_->IsShiftDown() ? 2.5f : 1.0f;
        if (!input_->IsControlDown())
        {
            if (camera->IsOrthographic())
                camera->SetOrthoSize(camera->GetOrthoSize() * ((input_->GetWheel() > 0) ? 1.1f : 0.9f));
            else
                cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldDirection() * ((input_->GetWheel() > 0) ? scale : -scale));
        }
    }

    const Settings* settings = Settings::GetInstance();
    if (input_->IsKeySequenceActive(settings->GetPage("Viewport")->Get("Move view forward")->value_.value<QKeySequence>()))
        cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldDirection() * 0.1f * multiplier);
    if (input_->IsKeySequenceActive(settings->GetPage("Viewport")->Get("Move view backward")->value_.value<QKeySequence>()))
        cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldDirection() * -0.1f * multiplier);
    if (input_->IsKeySequenceActive(settings->GetPage("Viewport")->Get("Move view left")->value_.value<QKeySequence>()))
        cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldRight() * -0.1f * multiplier);
    if (input_->IsKeySequenceActive(settings->GetPage("Viewport")->Get("Move view right")->value_.value<QKeySequence>()))
        cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldRight() * 0.1f * multiplier);
    if (input_->IsKeySequenceActive(settings->GetPage("Viewport")->Get("Move view up")->value_.value<QKeySequence>()))
        cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + Urho3D::Vector3(0, 1, 0) * 0.1f * multiplier);
    if (input_->IsKeySequenceActive(settings->GetPage("Viewport")->Get("Move view down")->value_.value<QKeySequence>()))
        cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + Urho3D::Vector3(0, 1, 0) * -0.1f * multiplier);
    
    if (input_->IsKeyPressed(Qt::Key::Key_Home))
    {
        cameraNode_->SetPosition(Vector3(0, 5, -15));
        cameraNode_->LookAt(Vector3(0, 0, 0));
        Quaternion camQuat = cameraNode_->GetWorldRotation();
        cameraPitch_ = camQuat.PitchAngle();
        cameraYaw_ = camQuat.YawAngle();
    }
    if (input_->IsKeyPressed(Qt::Key::Key_End))
    {   
        if (!view_->GetGizmos().empty())
        {
            Urho3D::Vector3 activeGizmoTrans;
            for (auto giz : view_->GetGizmos())
                activeGizmoTrans = giz->IsActive() ? giz->GetTransform().Translation() : activeGizmoTrans;

            cameraNode_->SetPosition(activeGizmoTrans - cameraNode_->GetWorldDirection() * 15);
        }
        else
        {
            // Otherwise just act like 'Home'
            cameraNode_->SetPosition(Vector3(0, 5, -15));
            cameraNode_->LookAt(Vector3(0, 0, 0));
            Quaternion camQuat = cameraNode_->GetWorldRotation();
            cameraPitch_ = camQuat.PitchAngle();
            cameraYaw_ = camQuat.YawAngle();
        }
    }
}

bool FlyController::CheckActivation(const ViewController* currentlyActive)
{
    if (currentlyActive)
        return currentlyActive == this;

    // is right mouse down?
    return input_->IsMousePressed(1) || input_->IsMousePressed(2);
}

bool FlyController::CheckTermination()
{
    return !input_->IsMousePressed(1) && !input_->IsMousePressed(2);
}

void FlyController::Update()
{
    const bool shiftHeld = input_->IsShiftDown();
    const bool middleMouse = input_->IsMousePressed(2);

    Urho3D::IntVector2 delta = input_->GetMouseDelta();

    const Settings* settings = Settings::GetInstance();
    const float invertModifier = settings->GetPage("Viewport")->Get("Invert mouse Y")->value_.toBool() ? -1.0f : 1.0f;
    cameraPitch_ += (delta.y_ * 0.1f * invertModifier);
    cameraYaw_ += delta.x_ * 0.1f;
    cameraPitch_ = Urho3D::Clamp(cameraPitch_, -90.0f, 90.0f);

    cameraNode_->SetWorldRotation(Quaternion(cameraPitch_, cameraYaw_, 0.0f));

    // Handle orbiting with shift held down or when using the middle mouse
    Vector3 orbitAround = Vector3(0, 0, 0);
    if (view_->GetGizmos().size())
    {
        for (auto giz : view_->GetGizmos())
            orbitAround = giz->IsActive() ? giz->GetTransform().Translation() : orbitAround;
    }
    if (middleMouse)
    {
        Vector3 d = cameraNode_->GetWorldPosition() - orbitAround;
        cameraNode_->SetWorldPosition(orbitAround - (cameraNode_->GetWorldRotation() * Vector3(0, 0, d.Length())));
    }
}

void FlyController::Prepare()
{

}

}