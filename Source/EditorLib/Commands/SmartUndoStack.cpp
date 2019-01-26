#include "SmartUndoStack.h"


bool SmartCommandQWrapper::CanMergeWith(const SmartCommand* rhs)
{
    if (const SmartCommandQWrapper* other = dynamic_cast<const SmartCommandQWrapper*>(rhs))
    {
        if (GetID() == other->GetID())
            return true;
    }
    return false;
}

bool SmartCommandQWrapper::MergeWith(const SmartCommand* rhs)
{
    if (const SmartCommandQWrapper* other = dynamic_cast<const SmartCommandQWrapper*>(rhs))
    {
        if (GetID() == other->GetID())
        {
            if (wrapped_->mergeWith(other->wrapped_))
                return true;
        }
    }
    return false;
}

SmartUndoStack::~SmartUndoStack()
{
    inDestructor_ = true;
    Clear();
}

void SmartUndoStack::Push(SmartCommand* command)
{
    if (command == 0x0)
        return;
    if (redoStack_.size())
    {
        for (auto it = redoStack_.rbegin(); it != redoStack_.rend(); ++it)
            delete *it;
        redoStack_.clear();
        emit CanRedoChanged(false);
    }

    // Command should do it's redo() work for being pushed first, so that it doesn't mesh with any merging.
    // If done after being added to the stack there could be issues with code that depends on IsFirstRedo().
    // Merging shouldn't care and should be written as if the floor could be pulled out from under at any time.
    command->Redo();

    if (undoStack_.size())
    {
        if (undoStack_.back()->GetID() == command->GetID())
        {
            if (!(undoStack_.back()->CanMergeWith(command) && undoStack_.back()->MergeWith(command)))
            {
                undoStack_.push_back(command);
                emit ActionAdded(command);
            }
            else
            {
                if (undoStack_.back()->ShouldBounce())
                {
                    SmartCommand* cmd = undoStack_.back();
                    undoStack_.pop_back();
                    emit ActionBounced(cmd);
                    delete cmd;
                }
                else
                    emit ActionMerged(undoStack_.back());
            }
        }
        else
        {
            undoStack_.push_back(command);
            emit ActionAdded(command);
        }
    }
    else
    {
        undoStack_.push_back(command);
        emit ActionAdded(command);
    }
}

void SmartUndoStack::Push(QUndoCommand* command)
{
    if (command == 0x0)
        return;
    SmartCommand* cmd = new SmartCommandQWrapper(command);
    Push(cmd);
}

void SmartUndoStack::Clear()
{
    ClearRedo();
    ClearUndo();
}

void SmartUndoStack::Undo(bool silent)
{
    if (undoStack_.size())
    {
        undoStack_.back()->Undo();
        redoStack_.push_back(undoStack_.back());
        undoStack_.pop_back();
        if (undoStack_.size() && !silent)
            undoStack_.back()->MadeCurrent();

        emit CanRedoChanged(!redoStack_.empty());
        emit CanUndoChanged(!undoStack_.empty());
        emit ActionUndone();
    }
}

void SmartUndoStack::Redo(bool silent)
{
    if (redoStack_.size())
    {
        redoStack_.back()->Redo();
        undoStack_.push_back(redoStack_.back());
        redoStack_.pop_back();
        if (undoStack_.size() && !silent)
            undoStack_.back()->MadeCurrent();

        emit CanRedoChanged(!redoStack_.empty());
        emit CanUndoChanged(!undoStack_.empty());
        emit ActionRedone();
    }
}

SmartCommand* SmartUndoStack::GetCommand(int index)
{
    if (index < GetCount())
    {
        if (index < undoStack_.size())
            return undoStack_[index];
        return redoStack_[index - undoStack_.size()];
    }
    return 0x0;
}

void SmartUndoStack::SetCurrent(int index)
{
    if (index < GetCount())
    {
        if (index < GetCurrent())
        {
            while (index < GetCurrent())
                Undo(true);
            if (undoStack_.size())
                undoStack_.back()->MadeCurrent();
        }
        else if (index > GetCurrent())
        {
            while (index > GetCurrent())
                Redo(true);
            if (undoStack_.size())
                undoStack_.back()->MadeCurrent();
        }
        emit IndexChanged(index);
    }
}

void SmartUndoStack::ClearRedo()
{
    if (redoStack_.size())
        for (auto it = redoStack_.begin(); it != redoStack_.end(); ++it)
            delete *it;
    redoStack_.clear();
    if (!inDestructor_)
        emit CanRedoChanged(false);
}

void SmartUndoStack::ClearUndo()
{
    if (undoStack_.size())
        for (auto it = undoStack_.rbegin(); it != undoStack_.rend(); ++it)
            delete *it;
    undoStack_.clear();
    if (!inDestructor_)
        emit CanUndoChanged(false);
}