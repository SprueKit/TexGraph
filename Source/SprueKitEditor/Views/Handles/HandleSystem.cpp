#include "HandleSystem.h"

HandleSystem::HandleSystem()
{

}

HandleSystem::HandleSystem(const std::vector<Handle*>& handles) :
    handles_(handles)
{

}

HandleSystem::~HandleSystem()
{
    for (unsigned i = 0; i < handles_.size(); ++i)
        delete handles_[i];
    handles_.clear();
}


void HandleSystem::MouseMoved(int x, int y)
{
    for (unsigned i = 0; i < handles_.size(); ++i)
        handles_[i]->MouseMoved(x, y);
}

bool HandleSystem::MousePressed(int x, int y, int button)
{
    for (unsigned i = 0; i < handles_.size(); ++i)
        if (handles_[i]->MousePressed(x, y, button))
            return true;
    return false;
}

void HandleSystem::MouseDragged(int x, int y, float deltaX, float deltaY, int button)
{
    for (unsigned i = 0; i < handles_.size(); ++i)
    {
        if (handles_[i]->HasFocus())
            handles_[i]->MouseDragged(x, y, deltaX, deltaY, button);
    }
}

void HandleSystem::MouseReleased(int x, int y, int button)
{
    for (unsigned i = 0; i < handles_.size(); ++i)
        handles_[i]->MouseReleased(x, y, button);
}

void HandleSystem::Paint(Urho3D::DebugRenderer* renderer)
{
    unsigned focusMask = 0;
    for (unsigned i = 0; i < handles_.size(); ++i)
        if (handles_[i]->HasHighlight() && handles_[i]->HighlightMask())
            focusMask &= handles_[i]->HighlightMask();

    for (unsigned i = 0; i < handles_.size(); ++i)
        handles_[i]->PaintHandle(renderer, focusMask & handles_[i]->GetBitmask());
}