#include "DocumentBase.h"

//#include "../Views/ViewBase.h"
#include <EditorLib/TaskProcessor.h>

DocumentBase::~DocumentBase()
{
    activeView_.reset();
    views_.clear();
    for (auto task : tasks_)
    {
        if (auto taskPtr = task.lock())
        {
            taskPtr->Cancel();
            taskPtr.reset();
        }
    }
}

void DocumentBase::AddDependentTask(std::shared_ptr<Task> task)
{
    tasks_.push_back(task);
}

int DocumentBase::GetActiveViewIndex() const
{
    for (unsigned i = 0; i < views_.size(); ++i)
        if (views_[i] == activeView_)
            return i;
    return -1;
}

void DocumentBase::SetDirty(bool state)
{
    dirty_ = state;
    emit DirtyChanged(state);
}