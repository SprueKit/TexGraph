#pragma once

#include "../ViewController.h"
#include "../Gizmo.h"

namespace SprueEditor
{
    /// Controller for handling Gizmo behavior
    class GizmoController : public ViewController
    {
    public:
        GizmoController(std::shared_ptr<Gizmo> giz, ViewManager*, ViewBase*, InputRecorder*);
        virtual ~GizmoController();

        virtual void PassiveChecks(const ViewController* currentActive) override;
        virtual bool CheckActivation(const ViewController* currentlyActive) override;
        virtual bool CheckTermination() override;
        virtual void Update() override;
        virtual void Terminate() override;

    protected:
        bool possible_; // if the mouse moves this gets set to false, unless we are active
        std::shared_ptr<Gizmo> gizmo_;
        GizmoAxis hitAxis_ = GizmoAxis::GA_None;
    };

}