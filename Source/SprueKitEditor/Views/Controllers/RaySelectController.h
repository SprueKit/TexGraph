#pragma once

#include "../ViewController.h"

#include <Urho3D/Math/Vector3.h>

#include <algorithm>

namespace SprueEngine
{
    class IEditable;
}

namespace SprueEditor
{
    /// Controller for handling mouse click selecting in the viewport
    class RaySelectController : public ViewController
    {
    public:
        RaySelectController(ViewManager* manager, ViewBase* view, InputRecorder* input) :
            ViewController(manager, view, input)
        {

        }

        virtual bool CheckActivation(const ViewController* currentlyActive) override;
        virtual bool CheckTermination() override;
        virtual void Update();

        SprueEngine::IEditable* GetRaycast() const;

        std::pair<Urho3D::Vector3, Urho3D::Vector3> GetPickRay() const;
    };

}