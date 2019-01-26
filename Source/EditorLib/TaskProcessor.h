#pragma once

#include <EditorLib/editorlib_global.h>

#include <memory>
#include <queue>

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QMutex>

class DocumentBase;

// Only enable_shared_from_this to work around Linker bugs, with TaskProcessor::AddTask(std::shared_ptr<Task>) not linking correctly.
class EDITORLIB_EXPORT Task : public std::enable_shared_from_this<Task>
{
private:
    friend class TaskProcessor;
    TaskProcessor* processor_;
public:
    Task(DocumentBase* doc) : document_(doc)
    {

    }
    virtual ~Task() { }

    /// Returns the pretty name of this task.
    virtual QString GetName() const = 0;
    /// Executed in the main thread. Perform any necessary preparation (that didn't make sense at construction time) in an override of this method.
    virtual void PrepareTask() { }
    /// Executed in the worker thread. Returns true if success. Implementations should not do dangerous or document dependent activities in this method.
    virtual bool ExecuteTask() { return true; }
    /// Executed in the main thread. Only called if the task has not been canceled.
    virtual void FinishTask() { }
    /// Implementation should return true if adding this task to the queue makes any existing tasks in the queue invalid.
    virtual bool Supercedes(Task* other) { return false; }

    /// If a Task is canceled it's FinishTask will never be called.
    void Cancel() { canceled_ = true; CancelDependentTask(); }
    /// Alias for canceling the chain of a task but not the results of the task itself.
    inline void SoftCancel() { CancelDependentTask(); }
    /// Returns true if the task has been canceled.
    bool IsCanceled() const { return canceled_; }

    std::shared_ptr<Task> GetDependentTask() { return dependentTask_; }
    void AddDependent(std::shared_ptr<Task> task) { dependentTask_ = task; }
    void CancelDependentTask();

protected:
    std::shared_ptr<Task> dependentTask_;
    bool canceled_ = false;
    DocumentBase* document_ = false;
    void SetProgress(float progress);
};

/// Only a queue by "intent," not function.
/// When performing successive chains of actions it is necessary to build a TaskQueue.
/// Consider undo/redo that may queue a successive series of light but redundant tasks,
/// by first queueing the tasks here redundancies can be eliminated before any tasks
/// are added to the TaskProcessor.
class EDITORLIB_EXPORT TaskQueue : private std::vector<std::shared_ptr<Task> >
{
public:
    typedef std::vector<std::shared_ptr<Task>> VecType;

    using VecType::size;
    using VecType::empty;
    using VecType::operator[];
    using VecType::begin;
    using VecType::end;
    using VecType::front;
    using VecType::back;

    void Add(std::shared_ptr<Task> task);
};

class EDITORLIB_EXPORT TaskProcessor : public QThread
{
    friend class Task;

    Q_OBJECT
public:
    /// Construct, if silen the TaskProcessor does not attempt to notify events.
    TaskProcessor(bool silent = false);
    /// Destruct.
    ~TaskProcessor();

    /// Adds a task to the queue, check for any ideal supercedure this task may have over existing tasks.
    void AddTask(std::shared_ptr<Task> addTask);
    void AddQueue(const TaskQueue& queue);

    /// Flushes the queue of completed tasks, performing their finalization tasks on the main thread.
    void Update();
    /// Gets the progress of the current task.
    float GetProgress() const { return progress_; }
    /// Gets the name of the current task.
    QString GetTaskName() const { return taskName_; }

    /// Performs the thread mainline.
    virtual void run() override;

signals:
    /// Notify the GUI about changes in the current task.
    void TaskChanged(const QString& txt, int ms = 1000);

private:
    /// Threading locker.
    QMutex lock_;
    QMutex completionLock_;
    /// List of pending tasks.
    std::vector< std::shared_ptr<Task> > tasks_;
    /// Lists of tasks that completed but are pending finalization.
    std::vector< std::shared_ptr<Task> > completed_;
    /// Timer used for schedule task finalization.
    QTimer* timer_;
    /// Name of current task.
    QString taskName_;
    /// Switch for whether this TaskProcessor is in silent-mode or not.
    bool silent_ = false;
    /// Progress of the current task.
    volatile float progress_;
};