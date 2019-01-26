#pragma once

#include <vector>


/// Derived controls (must be QWidget derived as well) will be persisted into the saved GUI state for layout save/restore
/// Very few controls are likely to have any need to do this (their docks will be sufficient)
class IPersistedWidget
{
public:
    IPersistedWidget()
    {
        list_.push_back(this);
    }

    virtual ~IPersistedWidget()
    {
        auto found = std::find(list_.begin(), list_.end(), this);
        if (found != list_.end())
            list_.erase(found);
    }

    static std::vector<IPersistedWidget*>& GetList() { return list_; }

private:
    static std::vector<IPersistedWidget*> list_;
};
