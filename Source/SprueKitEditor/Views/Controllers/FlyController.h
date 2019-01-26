#pragma once

#include "../ViewController.h"

#include <Urho3D/Scene/Node.h>

namespace SprueEditor
{

    /// Controller for fly/orbit in 3d camera management
    class FlyController : public ViewController
    {
    public:
        FlyController(Urho3D::Node* cameraNode, ViewManager* manager, ViewBase* view, InputRecorder* input);

        virtual void PassiveChecks(const ViewController* currentActive) override;
        virtual bool CheckActivation(const ViewController* currentlyActive) override;
        virtual bool CheckTermination() override;
        virtual void Update();

        /// Must grab the current mouse position
        virtual void Prepare();
    private:
        Urho3D::Node* cameraNode_;
        float cameraPitch_;
        float cameraYaw_;
    };

}