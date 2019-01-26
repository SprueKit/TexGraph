#pragma once

namespace SprueEditor
{
    class InputRecorder;
    class ViewManager;
    class ViewBase;

    /// Abstracts the handling of control inside a view
    class ViewController
    {
    public:
        ViewController(ViewManager* manager, ViewBase* view, InputRecorder* input) :
            viewManager_(manager),
            view_(view),
            input_(input)
        {

        }
        
        /// Do anything passive (likely just keyboard activity)
        virtual void PassiveChecks(const ViewController* currentActive) { }
        /// Invoked like a passive-check except only when this view-controller is disabled.
        virtual void DisabledChecked(const ViewController* currentActive) { }
        /// Check whether this controller should be used or not, given the current controller for the option to preempt (ie fly->orbit or orbit->fly)
        virtual bool CheckActivation(const ViewController* currentlyActive) = 0;
        /// Check whether it's time to terminate this controller (mouse no longer, down, etc)
        virtual bool CheckTermination() = 0;

        /// Update the controller, performing any work required
        virtual void Update() = 0;

        /// Do any preparation when this controller is activated
        virtual void Prepare() { }
        /// Do any termination cleanup
        virtual void Terminate() { }

        /// Disabled view controllers are not considered.
        virtual bool IsEnabled() const { return enabled_; }
        /// Change the enabled/disabled state of this view controller. Derived types may have consequences.
        virtual void SetEnabled(bool enabled) { enabled_ = enabled; }

    protected:
        ViewBase* view_ = 0x0;
        ViewManager* viewManager_ = 0x0;
        InputRecorder* input_ = 0x0;
        bool enabled_ = true;
    };

}