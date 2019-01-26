#include "GizmoController.h"

#include "../InputRecorder.h"
#include "../RenderWidget.h"
#include "../../UrhoHelpers.h"
#include "../ViewBase.h"
#include "../ViewManager.h"

#include <EditorLib/LogFile.h>
#include <EditorLib/Selectron.h>

#include <SprueEngine/StringHash.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace SprueEditor
{

    GizmoController::GizmoController(std::shared_ptr<Gizmo> giz, ViewManager* manager, ViewBase* view, InputRecorder* input) :
        ViewController(manager, view, input),
        possible_(false)
    {
        gizmo_ = giz;
    }

    GizmoController::~GizmoController()
    {
        
    }

    void GizmoController::PassiveChecks(const ViewController* currentActive)
    {
        // Don't do anything if we're active, otherwise we risk breaking the axis mode
        if (currentActive == this)
            return;

        return;
    }

    bool GizmoController::CheckActivation(const ViewController* currentlyActive)
    {
        if (!gizmo_->IsActive())
            return false;
        // We never preempt anything
        if (currentlyActive && currentlyActive != this)
            return false;

        // If we're active then don't bother with ray checking
        if (currentlyActive == this)
            return input_->IsMousePressed(0);

        // Check for handle hit
        auto graphics = view_->GetContext()->GetSubsystem<Graphics>();
        IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
        if (input_->IsMousePressed(0))
            return gizmo_->CheckMouse(vec.x_, vec.y_);
        else
            gizmo_->MouseMove(vec.x_, vec.y_);
        return false;
    }

    bool GizmoController::CheckTermination()
    {
        if (!input_->IsMousePressed(0))
        {
            auto graphics = view_->GetContext()->GetSubsystem<Graphics>();
            IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
            gizmo_->MouseUp(vec.x_, vec.y_);
            return true;
        }
        return false;
    }

    void GizmoController::Update()
    {
        auto graphics = view_->GetContext()->GetSubsystem<Graphics>();
        IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
        const float deltaX = input_->GetMouseDelta().x_;
        const float deltaY = input_->GetMouseDelta().y_;

        gizmo_->RecordTransforms();
        auto oldTransform = gizmo_->GetTransform();
        gizmo_->MouseDrag(vec.x_ + deltaX, vec.y_ + deltaY, deltaX, deltaY);
        {
            Urho3D::Matrix3x4 transMat = gizmo_->GetEditTransform();
            gizmo_->ApplySnapping(transMat);
            gizmo_->ApplyTransform(transMat);
            view_->GizmoUpdated(gizmo_);

            if (view_->GetDocument())
                gizmo_->PushUndo(view_->GetDocument(), oldTransform);
            
            Selectron* sel = viewManager_->GetRenderWidget()->GetSelectron();
            gizmo_->NotifyDataChanged((void*)viewManager_->GetRenderWidget(), sel);            
            view_->VisuallyChanged(sel);
        }
    }

    void GizmoController::Terminate()
    {
        
    }

}