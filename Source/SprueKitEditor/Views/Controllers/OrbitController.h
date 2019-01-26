#pragma once

#include "../ViewController.h"

#include <Urho3D/Scene/Node.h>

namespace SprueEditor
{

    /// Controls a camera whose only capability is to Orbit around a center. Basically a duplicate of FlyController with flight stripped out.
    class OrbitController : public ViewController
    {
    public:
        OrbitController(Urho3D::Node* cameraNode, ViewManager* manager, ViewBase* view, InputRecorder* input);

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