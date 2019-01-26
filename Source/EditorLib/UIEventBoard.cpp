#include "UIEventBoard.h"

std::vector<UIEventBoard*> UIEventBoard::receivers_;
std::vector<UIEvent*> UIEventBoard::events_;


UIEventBoard::UIEventBoard()
{
    receivers_.push_back(this);
}

UIEventBoard::~UIEventBoard()
{
    auto found = std::find(receivers_.begin(), receivers_.end(), this);
    if (found != receivers_.end())
        receivers_.erase(found);
}

void UIEventBoard::ProcessEvents()
{
    for (unsigned rIdx = 0; rIdx < receivers_.size(); ++rIdx)
    {
        auto recv = receivers_[rIdx];
        for (unsigned eIdx = 0; eIdx < events_.size(); ++eIdx)
            recv->ProcessEvent(events_[eIdx]);
    }

    for (auto e : events_)
        delete e;
    events_.clear();
}

void UIEventBoard::PushEvent(UIEvent* event)
{
    events_.push_back(event);
}
