#pragma once

#include "ViewController.h"

#include <memory>
#include <vector>

namespace SprueEditor
{
    /// Encapsulates multiple view-controllers into a singular controller (only a single contained controller will actually be active at a time).
    /// Using a MultiViewController simplifies working with sets of interelated view-controllers that need to come and go based on UI/etc.
    class MultiViewController : public ViewController
    {
    public:
        /// Construct.
        MultiViewController(ViewManager* manager, ViewBase* view, InputRecorder* input);

        /// Adds a view controller to this set.
        void AddController(std::shared_ptr<ViewController> controller);
        /// Removes a view controller from this set.
        void RemoveController(std::shared_ptr<ViewController> controller);
        /// Returns the active view controller contained in this set.
        std::shared_ptr<ViewController> GetActiveController() { return active_; }

        // ViewController implementation
        /// Perform checks that must always be performed (called after CheckActivation)
        virtual void PassiveChecks(const ViewController* currentActive) override;
        /// Test whether any of the contained controllers need to become the active one.
        virtual bool CheckActivation(const ViewController* currentlyActive) override;
        /// Checks with the currently active controller needs to terminate.
        virtual bool CheckTermination() override;
        /// Runs any updates that need to be performed while the controller is active.
        virtual void Update() override;
        /// Allows the active controller to do any preparation work before updating.
        virtual void Prepare() override;
        /// Allow the active controller to perform any necessary cleanup when deactivated.
        virtual void Terminate() override;

    protected:
        /// Locally tracked active controller within this set.
        std::shared_ptr<ViewController> active_;
        /// List of all controllers contained in this multi-controller.
        std::vector<std::shared_ptr<ViewController> > controllers_;
    };

}