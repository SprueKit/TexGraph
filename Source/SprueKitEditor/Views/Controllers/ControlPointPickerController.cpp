#include "ControlPointPickerController.h"

#include "../InputRecorder.h"
#include "../ViewBase.h"

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Math/Vector2.h>

using namespace Urho3D;

namespace SprueEditor
{

    static float CalculateSphereScreenFactor(Urho3D::Vector3 vec, Urho3D::Camera* camera)
    {
        Urho3D::Matrix4 viewproj = camera->GetProjection() * camera->GetView();
        Urho3D::Vector4 transform = Urho3D::Vector4(vec.x_, vec.y_, vec.z_, 1.0f);
        transform = viewproj * transform;
        float scale = transform.w_ * 0.2f;
        if (scale <= 0.0f)
            return 1.0f;
        return scale;
    }

    ControlPointPickerController::ControlPointPickerController(Urho3D::Node* cameraNode, ViewManager* manager, ViewBase* view, InputRecorder* input) :
        ViewController(manager, view, input),
        camera_(cameraNode)
    {

    }

    void ControlPointPickerController::PassiveChecks(const ViewController* currentActive)
    {
        
    }

    bool ControlPointPickerController::CheckActivation(const ViewController* currentlyActive)
    {
        if (currentlyActive)
            return false;
        
        IntVector2 vec = view_->GetContext()->GetSubsystem<Input>()->GetMousePosition();
        const float pickX = vec.x_;
        const float pickY = vec.y_;

        float screenHeight = view_->GetContext()->GetSubsystem<Graphics>()->GetHeight();
        float screenWidth = view_->GetContext()->GetSubsystem<Graphics>()->GetWidth();
        auto cameraNode = view_->GetCamera();
        auto camera = view_->GetCamera()->GetComponent<Camera>();
        Urho3D::Ray ray = camera->GetScreenRay(((float)pickX) / screenWidth, ((float)pickY) / screenHeight);

        // was the left mouse previously pressed?
        if (input_->WasMousePressed(0))
        {
            auto gizmos = view_->GetGizmos();
            Urho3D::Sphere sphere;
            sphere.radius_ = 0.25f;
            
            float shortestDistance = M_INFINITY;
            std::shared_ptr<Gizmo> bestGizmo;
            for (auto giz : gizmos)
            {
                if (!giz->IsActive())
                {
                    Urho3D::Matrix3x4 mat = giz->GetTransform();
                    sphere.center_ = mat.Translation();

                    float sphereScale = CalculateSphereScreenFactor(sphere.center_, camera);
                    sphere.radius_ = 0.08f * sphereScale;

                    float hitDist = ray.HitDistance(sphere);
                    if (hitDist < shortestDistance)
                    {
                        shortestDistance = hitDist;
                        bestGizmo = giz;
                    }
                }
            }
            
            if (bestGizmo)
            {
                for (auto giz : gizmos)
                {
                    if (giz != bestGizmo)
                        giz->SetActive(false);
                    else
                        giz->SetActive(true);
                }
                return true;
            }
        }
        return false;
    }

    bool ControlPointPickerController::CheckTermination()
    {
        return true;
    }

    void ControlPointPickerController::Update()
    {

    }

}