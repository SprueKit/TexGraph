#pragma once

#include <EditorLib/editorlib_global.h>

#include <QObject>
#include <QUndoCommand>

#include <stack>

/// Used in place of QUndoCommand because some extra functionality is needed to prevent whacky behaviour from occuring.
/// The whackiness (particularly with notifying of data changes) is bad enough to not be worth tolerating.
class EDITORLIB_EXPORT SmartCommand
{
public:
    /// Construct.
    SmartCommand() { }
    /// Destruct.
    virtual ~SmartCommand() { }

    /// Retreive the display text.
    virtual QString GetText() const = 0;

    /// Called when this action is being reperformed. Call this base method AFTER you've done your work.
    virtual void Redo() { lastActionWasUndo_ = false; ++redoCount_; }
    /// Called when this action is being reversed. Call this base method AFTER you've done your work.
    virtual void Undo() { lastActionWasUndo_ = true; }
    /// Called when undo/redo results in this action being the 'current' one at the top (topmost action in 'redo' condition).
    virtual void MadeCurrent() { }

    /// Override to return an ID that will be checked first before considering merging.
    virtual int GetID() const = 0;

    /// Preemptive check for whether to even consider merging or not
    virtual bool CanMergeWith(const SmartCommand* rhs) = 0;
    /// Check if merging is allowed, implementation must also do the merge and return true if the merge was performed.
    virtual bool MergeWith(const SmartCommand* rhs) { return false; }

    /// After a successful merge is done this will be called to check if the merger results in Old == New, in which case the command will be removed from the stack.
    /// Destructors must be aware of this if they perform sensitive operations as the command will be deleted.
    virtual bool ShouldBounce() { return false; }

    /// Returns true if the last usage of this object was to perform an undo.
    bool LastActionWasUndo() const { return lastActionWasUndo_; }
    /// Returns true if the action is currently in it's first call of Redo().
    bool IsFirstRedo() const { return redoCount_ == 0; }

protected:
    /// Marks whether the last action was an undo or a redo.
    bool lastActionWasUndo_ = false;
    /// Marks whether the action has been redone at least once already. Count enables special handling in MadeCurrent().
    int redoCount_ = 0;
};

class EDITORLIB_EXPORT SimpleSmartCommand : public SmartCommand
{
public:
    virtual QString GetText() const override { return text_; }
    void SetText(const QString& txt) { text_ = txt; }
protected:
    QString text_;
};

/// Implementation of SprueCommand that wraps a QUndoCommand.
class EDITORLIB_EXPORT SmartCommandQWrapper : public SmartCommand
{
public:
    SmartCommandQWrapper(QUndoCommand* cmd) : wrapped_(cmd)
    {

    }
    virtual ~SmartCommandQWrapper() { delete wrapped_; }
    virtual QString GetText() const override { return wrapped_->text(); }
    virtual void Redo() override { wrapped_->redo(); SmartCommand::Redo(); }
    virtual void Undo() override { wrapped_->undo(); SmartCommand::Undo(); }
    virtual int GetID() const override { return wrapped_->id(); }
    virtual bool CanMergeWith(const SmartCommand* rhs) override;
    virtual bool MergeWith(const SmartCommand* rhs) override;

private:
    QUndoCommand* wrapped_;
};

/// Used instead of QUndoStack in order to have a bit more power over the undo-stack and use SprueCommand's capabilities.
class EDITORLIB_EXPORT SmartUndoStack : public QObject
{
    Q_OBJECT
public:
    /// Construct.
    SmartUndoStack() { }
    /// Destruct.
    virtual ~SmartUndoStack();

    /// Adds an item to the undo stack. The redo stack will be cleared.
    void Push(SmartCommand* command);
    // TODO: remove
    void push(SmartCommand* command) { Push(command); }
    /// Adds (and wrap) a QUndoCommand to the stack. The redo stack will be cleared.
    void Push(QUndoCommand* command);
    /// TODO: remove
    void push(QUndoCommand* command) { Push(command); }

    /// Clears both the redo/undo stacks.
    void Clear();
    /// Returns true if there is anything can be undone.
    bool CanUndo() const { return undoStack_.size() > 0; }
    /// Returns true if there is anything that can be redone.
    bool CanRedo() const { return redoStack_.size() > 0; }
    /// Undo the operation at the top of the undo-stack. Silent will not call SprueCommand::MadeCurrent().
    void Undo(bool silent = false);
    /// Redo the operation at the top of the redo-stack. Silent will not call SprueCommand::MadeCurrent().
    void Redo(bool silent = false);

    QString GetUndoText() const { return undoStack_.empty() ? QString() : undoStack_.back()->GetText(); }
    QString GetRedoText() const { return redoStack_.empty() ? QString() : redoStack_.back()->GetText(); }

    /// Gets the number of items in the undo stack.
    int GetCount() const { return undoStack_.size() + redoStack_.size(); }
    /// Gets the current index in the undo stack.
    int GetCurrent() const { return undoStack_.size() - 1; }
    /// Gets the command at the given index.
    SmartCommand* GetCommand(int index);
    /// Sets the current index, will perform undo/redo as needed to get there.
    void SetCurrent(int index);

signals:
    void ActionAdded(SmartCommand*);
    void ActionBounced(SmartCommand*);
    void ActionMerged(SmartCommand*);
    void IndexChanged(int index);
    void ActionUndone();
    void ActionRedone();
    void CanUndoChanged(bool);
    void CanRedoChanged(bool);

private:
    /// Clears the redo stack.
    void ClearRedo();
    /// Clears the undo stack.
    void ClearUndo();

    /// Not an actual stack, convenience reasons.
    std::vector<SmartCommand*> redoStack_;
    /// Not an actual stack, convenience reasons.
    std::vector<SmartCommand*> undoStack_;
    bool inDestructor_ = false;
};