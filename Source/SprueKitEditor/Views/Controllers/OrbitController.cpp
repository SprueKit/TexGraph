#include "OrbitController.h"

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

    OrbitController::OrbitController(Urho3D::Node* cameraNode, ViewManager* manager, ViewBase* view, InputRecorder* input) :
        ViewController(manager, view, input),
        cameraNode_(cameraNode),
        cameraPitch_(0.0f),
        cameraYaw_(0.0f)
    {
        Quaternion camQuat = cameraNode_->GetWorldRotation();
        cameraPitch_ = camQuat.PitchAngle();
        cameraYaw_ = camQuat.YawAngle();
    }

    void OrbitController::PassiveChecks(const ViewController* currentActive)
    {
        const bool shiftHeld = input_->IsShiftDown();
        const float multiplier = shiftHeld ? 5.0f : 1.0f;

        if (input_->GetWheel())
        {
            const float scale = input_->IsShiftDown() ? 2.5f : 1.0f;
            cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldDirection() * ((input_->GetWheel() > 0) ? scale : -scale));
        }

        const Settings* settings = Settings::GetInstance();
        if (input_->IsKeySequenceActive(settings->GetValue("Viewport/Move view forward")->value_.value<QKeySequence>()))
            cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldDirection() * 0.1f * multiplier);
        if (input_->IsKeySequenceActive(settings->GetValue("Viewport/Move view backward")->value_.value<QKeySequence>()))
            cameraNode_->SetWorldPosition(cameraNode_->GetWorldPosition() + cameraNode_->GetWorldDirection() * -0.1f * multiplier);

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
                cameraNode_->SetPosition(view_->GetGizmos()[0]->GetTransform().Translation() - cameraNode_->GetWorldDirection() * 15);
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

    bool OrbitController::CheckActivation(const ViewController* currentlyActive)
    {
        if (currentlyActive)
            return currentlyActive == this;

        // is right mouse down?
        return input_->IsMousePressed(1) || input_->IsMousePressed(2);
    }

    bool OrbitController::CheckTermination()
    {
        return !input_->IsMousePressed(1) && !input_->IsMousePressed(2);
    }

    void OrbitController::Update()
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
        
        Vector3 d = cameraNode_->GetWorldPosition() - orbitAround;
        cameraNode_->SetWorldPosition(orbitAround - (cameraNode_->GetWorldRotation() * Vector3(0, 0, d.Length())));
    }

    void OrbitController::Prepare()
    {

    }

}