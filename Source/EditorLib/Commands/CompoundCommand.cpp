#include "CompoundCommand.h"

CompoundCommand::CompoundCommand(const QString& text, std::vector<SmartCommand*> commands) :
    commands_(commands)
{
    SetText(text);
}

CompoundCommand::~CompoundCommand()
{
    for (auto cmd : commands_)
        delete cmd;
}

void CompoundCommand::Redo()
{
    SimpleSmartCommand::Redo();
    for (auto cmd : commands_)
        cmd->Redo();
}
void CompoundCommand::Undo()
{
    SimpleSmartCommand::Undo();
    for (auto cmd : commands_)
        cmd->Undo();
}

void CompoundCommand::MadeCurrent()
{
    for (auto cmd : commands_)
        cmd->MadeCurrent();
}

int CompoundCommand::GetID() const
{
    return INT_MAX;
}

bool CompoundCommand::CanMergeWith(const SmartCommand* rhs)
{
    if (auto other = dynamic_cast<const CompoundCommand*>(rhs))
    {
        if (other->commands_.size() == commands_.size())
        {
            bool canMerge = true;
            for (int i = 0; i < commands_.size(); ++i)
                canMerge &= commands_[i]->CanMergeWith(other->commands_[i]);
            return canMerge;
        }
    }
    return false;
}

bool CompoundCommand::MergeWith(const SmartCommand* rhs)
{
    // TODO: is this reconcilable?
    if (auto other = dynamic_cast<const CompoundCommand*>(rhs))
    {
        if (other->commands_.size() == commands_.size())
        {
            // possible
            for (int i = 0; i < commands_.size(); ++i)
                commands_[i]->MergeWith(other->commands_[i]);
            return true;
        }
    }
    return false;
}

bool CompoundCommand::ShouldBounce()
{
    // Bounce if any command says to bounce
    for (auto cmd : commands_)
        if (!cmd->ShouldBounce())
            return false;
    return true;
}