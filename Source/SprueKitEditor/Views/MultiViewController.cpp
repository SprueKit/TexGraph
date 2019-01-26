#include "MultiViewController.h"

namespace SprueEditor
{

    MultiViewController::MultiViewController(ViewManager* manager, ViewBase* view, InputRecorder* input) :
        ViewController(manager, view, input)
    {

    }

    void MultiViewController::AddController(std::shared_ptr<ViewController> controller)
    {
        auto found = std::find(controllers_.begin(), controllers_.end(), controller);
        if (found != controllers_.end())
            controllers_.push_back(controller);
    }

    void MultiViewController::RemoveController(std::shared_ptr<ViewController> controller)
    {
        auto found = std::find(controllers_.begin(), controllers_.end(), controller);
        if (found != controllers_.end())
            controllers_.erase(found);
    }

    void MultiViewController::PassiveChecks(const ViewController* currentActive)
    {
        // Index iteration allows for modification
        for (unsigned i = 0; i < controllers_.size(); ++i)
            controllers_[i]->PassiveChecks(currentActive);
    }

    bool MultiViewController::CheckActivation(const ViewController* currentlyActive)
    {
        for (auto controller : controllers_)
        {
            if (controller->CheckActivation(currentlyActive))
            {
                active_ = controller;
                return true;
            }
        }
        return false;
    }

    bool MultiViewController::CheckTermination()
    {
        if (active_)
            return active_->CheckTermination();
        return true;
    }

    void MultiViewController::Update()
    {
        if (active_)
            active_->Update();
    }

    void MultiViewController::Prepare()
    {
        if (active_)
            active_->Prepare();
    }

    void MultiViewController::Terminate()
    {
        if (active_)
            active_->Terminate();
        active_.reset();
    }

}