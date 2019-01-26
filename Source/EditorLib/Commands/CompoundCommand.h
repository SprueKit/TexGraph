#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/Commands/SmartUndoStack.h>

#include <QString>

#include <vector>

/// Groups together multiple commands into a single command.
/// Example use: when a gizmo moves multiple objects.
class EDITORLIB_EXPORT CompoundCommand : public SimpleSmartCommand
{
public:
    /// Construct with a set of commands.
    CompoundCommand(const QString& text, std::vector<SmartCommand*> commands);
    /// Destruct.
    virtual ~CompoundCommand();

    virtual void Redo() override;
    virtual void Undo()  override;
    virtual void MadeCurrent() override;
    virtual int GetID() const override;
    virtual bool CanMergeWith(const SmartCommand* rhs) override;
    virtual bool MergeWith(const SmartCommand* rhs) override;
    virtual bool ShouldBounce() override;

protected:
    std::vector<SmartCommand*> commands_;
};