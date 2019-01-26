#pragma once

#include "../ViewController.h"

#include <Urho3D/Scene/Node.h>

namespace SprueEditor
{

    class ControlPointPickerController : public ViewController
    {
    public:
        ControlPointPickerController(Urho3D::Node* cameraNode, ViewManager* manager, ViewBase* view, InputRecorder* input);

        virtual void PassiveChecks(const ViewController* currentActive) override;
        virtual bool CheckActivation(const ViewController* currentlyActive) override;
        virtual bool CheckTermination() override;
        virtual void Update();

    protected:
        Urho3D::Node* camera_;
    };

}