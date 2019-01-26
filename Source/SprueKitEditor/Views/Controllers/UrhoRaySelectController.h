#pragma once

#include "../ViewController.h"

#include <Urho3D/Scene/Scene.h>

namespace SprueEditor
{
    /// Controller for handling mouse click selecting in the viewport
    class UrhoRaySelectController : public ViewController
    {
    public:
        UrhoRaySelectController(ViewManager* manager, ViewBase* view, InputRecorder* input, Urho3D::Scene* scene) :
            ViewController(manager, view, input),
            scene_(scene)
        {

        }

        virtual bool CheckActivation(const ViewController* currentlyActive) override;
        virtual bool CheckTermination() override;
        virtual void Update();

        Urho3D::Node* GetRaycast() const;

    private:
        Urho3D::Scene* scene_;
    };

}