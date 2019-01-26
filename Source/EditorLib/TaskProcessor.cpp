#include "TaskProcessor.h"

#include <qobject.h>
#include <chrono>

#define LOCK(WHO) { WHO.lock(); 
#define UNLOCK(WHO) WHO.unlock(); }

void Task::CancelDependentTask()
{
    if (dependentTask_)
    {
        dependentTask_->Cancel();
        dependentTask_.reset();
    }
}

TaskProcessor::TaskProcessor(bool silent) :
    silent_(silent)
{
    timer_ = new QTimer();
    timer_->start(50);
    connect(timer_, &QTimer::timeout, [=]() {
        Update();
    });
}

TaskProcessor::~TaskProcessor()
{
    //thread_->detach();
    //delete thread_;
    delete timer_;
}

void TaskProcessor::run()
{
    for (;;)
    {
        std::shared_ptr<Task> currentTask;
        PROCESS_TASK:
        LOCK(lock_);
            if (tasks_.size() > 0 || currentTask)
            {
                if (!currentTask)
                {
                    currentTask = tasks_.front();
                    currentTask->processor_ = this;
                    tasks_.erase(tasks_.begin());
                }
                currentTask->PrepareTask();
                if (!silent_)
                    emit TaskChanged(QString("%1...").arg(currentTask->GetName()));
            }
        UNLOCK(lock_);
            if (currentTask)
            {
                currentTask->ExecuteTask();

                LOCK(completionLock_);
                    completed_.push_back(currentTask);
                    if (currentTask->GetDependentTask())
                        currentTask = currentTask->GetDependentTask();
                    else
                        currentTask.reset();
                emit TaskChanged(QString());
                UNLOCK(completionLock_);
                if (currentTask)
                    goto PROCESS_TASK;
                else // we just processed, so even if there isn't a dependent we want to continue onward if there's tasks in the queue
                    goto PROCESS_TASK;
            }
            else // If we had a task then don't wait to check for the next one.
                msleep(36);
    }
}

void TaskProcessor::AddTask(std::shared_ptr<Task> task)
{
    if (!task.get())
        return;
    LOCK(lock_);
        // Check to see we force anyone to be removed
        for (unsigned i = 0; i < tasks_.size(); ++i)
        {
            if (task->Supercedes(tasks_[i].get()))
            {
                /// Cancel so we properly clean up
                tasks_[i]->Cancel();
                tasks_[i].reset();
                tasks_.erase(tasks_.begin() + i);
                --i;
            }
        }
    tasks_.push_back(task);
    //task->PrepareTask();
    UNLOCK(lock_);
}

void TaskProcessor::AddQueue(const TaskQueue& queue)
{
    LOCK(lock_);
    for (size_t i = 0; i < queue.size(); ++i)
        AddTask(queue[i]);
    UNLOCK(lock_);
}

void TaskProcessor::Update()
{
    LOCK(completionLock_);
        for (unsigned i = 0; i < completed_.size(); ++i)
        {
            if (!completed_[i]->IsCanceled())
            {
                completed_[i]->FinishTask();
                completed_[i].reset();
            }
        }
    completed_.clear();
    UNLOCK(completionLock_);
}

void Task::SetProgress(float progress)
{
    processor_->progress_ = progress;
}

void TaskQueue::Add(std::shared_ptr<Task> task)
{
    if (!task.get())
        return;
    
    // Check to see we force anyone to be removed
    for (unsigned i = 0; i < size(); ++i)
    {
        if (task->Supercedes((*this)[i].get()))
        {
            (*this)[i].reset();
            (*this).erase(begin() + i);
            --i;
        }
    }
    push_back(task);
}