#pragma once

#include <QWidget>

#include <vector>


/// Interface type containing an autolist of QWidgets that are desirable to be able to seek out by name or type
/// Specific use is finding a given control without hunting through the UI or main window to find it
class ISignificantControl
{
public:
    ISignificantControl()
    {
        list_.push_back(this);
    }

    virtual ~ISignificantControl()
    {
        auto found = std::find(list_.begin(), list_.end(), this);
        if (found != list_.end())
            list_.erase(found);
    }

    static std::vector<ISignificantControl*>& GetList() { return list_; }

    template<class T>
    static T* GetControl()
    {
        for (auto item : list_)
        {
            if (T* ret = dynamic_cast<T*>(item))
                return ret;
        }
        return 0x0;
    }

    /// Find a specific QWidget dervied object by name
    template<class T>
    static T* GetByName(const std::string& name)
    {
        for (auto item : list_)
        {
            if (QWidget* ret = dynamic_cast<QWidget*>(item))
            {
                if (ret->objectName().compare(name.c_str()) == 0)
                    return dynamic_cast<T*>(ret);
            }
        }
        return 0x0;
    }

private:
    static std::vector<ISignificantControl*> list_;
};