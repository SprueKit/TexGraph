#include "PanZoomController.h"

#include "../InputRecorder.h"
#include "../ViewBase.h"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

namespace SprueEditor
{

PanZoomController::PanZoomController(ViewManager* manager, ViewBase* view, InputRecorder* input) :
    ViewController(manager, view, input)
{

}

void PanZoomController::PassiveChecks(const ViewController* currentActive)
{
    const bool shiftHeld = input_->IsShiftDown();

    Urho3D::Node* camera = view_->GetCamera();
    Urho3D::Camera* cam = camera->GetComponent<Urho3D::Camera>();
    Urho3D::Vector3 cameraPos = camera->GetWorldPosition();

    float zoomFactor = cam->GetZoom();
    float scalingBias = 1.0f / zoomFactor;
    float moveMultiplierBias = shiftHeld ? 0.2f : 0.05f;

    // X/Y position
    if (input_->IsKeyPressed(Qt::Key::Key_Up) || input_->IsKeyPressed(Qt::Key::Key_W))
    {
        if (isOverhead_)
            cameraPos.z_ += 1.0f * moveMultiplierBias * scalingBias;
        else
            cameraPos.y_ += 1.0f * moveMultiplierBias * scalingBias;
    }
    if (input_->IsKeyPressed(Qt::Key::Key_Down) || input_->IsKeyPressed(Qt::Key::Key_S))
    {
        if (isOverhead_)
            cameraPos.z_ -= 1.0f * moveMultiplierBias * scalingBias;
        else
            cameraPos.y_ -= 1.0f * moveMultiplierBias * scalingBias;
    }
    if (input_->IsKeyPressed(Qt::Key::Key_Left) || input_->IsKeyPressed(Qt::Key::Key_A))
        cameraPos.x_ -= 1.0f * moveMultiplierBias * scalingBias;
    if (input_->IsKeyPressed(Qt::Key::Key_Right) || input_->IsKeyPressed(Qt::Key::Key_D))
        cameraPos.x_ += 1.0f * moveMultiplierBias * scalingBias;

    // Zoom
    if (input_->IsKeyPressed(Qt::Key::Key_Equal) || input_->IsKeyPressed(Qt::Key::Key_Plus) || input_->IsKeyPressed(Qt::Key::Key_E)) // Zoom in
        cam->SetZoom(std::max(cam->GetZoom() + 0.01f, 0.1f));
    if (input_->IsKeyPressed(Qt::Key::Key_Minus) || input_->IsKeyPressed(Qt::Key::Key_C)) // Zoom out
        cam->SetZoom(std::max(cam->GetZoom() - 0.01f, 0.1f));
    if (input_->GetWheel())
        cam->SetZoom(std::max(cam->GetZoom() + 0.01f * input_->GetWheel(), 0.1f));

    // Reset
    if (input_->IsKeyPressed(Qt::Key::Key_Home))
    {
        if (isOverhead_)
            cameraPos.x_ = cameraPos.z_ = 0.0f;
        else
            cameraPos.x_ = cameraPos.y_ = 0.0f;
    }
    
    camera->SetWorldPosition(cameraPos);
}

bool PanZoomController::CheckActivation(const ViewController* currentlyActive)
{
    const bool shiftHeld = input_->IsShiftDown();
    return input_->IsMousePressed(2) || input_->IsMousePressed(1);
}

bool PanZoomController::CheckTermination()
{
    const bool shiftHeld = input_->IsShiftDown();
    return !input_->IsMousePressed(2) && !input_->IsMousePressed(1);
}

void PanZoomController::Update()
{
    const bool shiftHeld = input_->IsShiftDown();

    Urho3D::IntVector2 delta = input_->GetMouseDelta();

    Urho3D::Node* camera = view_->GetCamera(); 
    float zoomFactor = camera->GetOrCreateComponent<Urho3D::Camera>()->GetZoom();
    float scalingBias = 1.0f / zoomFactor;
    float multiplierBias = shiftHeld ? 0.2f : 0.05f;

    Urho3D::Vector3 cameraPos = camera->GetWorldPosition();
    cameraPos.x_ -= delta.x_ * multiplierBias * scalingBias;
    if (isOverhead_)
        cameraPos.z_ += delta.y_ * multiplierBias * scalingBias;
    else
        cameraPos.y_ += delta.y_ * multiplierBias * scalingBias;
    camera->SetWorldPosition(cameraPos);
}


}