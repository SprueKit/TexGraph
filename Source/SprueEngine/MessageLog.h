#pragma once

#include <string>

namespace SprueEngine
{

enum LogLevel
{
    LL_ERROR = 0,
    LL_WARNING = 1,
    LL_INFO = 2,
    LL_DEBUG = 3
};

typedef void (*LOG_CALLBACK)(const char*, int);

class MessageLog
{
public:
    MessageLog();
    ~MessageLog();

    void Log(const std::string& msg, LogLevel level)
    {
        if (callback_)
            callback_(msg.c_str(), (int)level);
    }

    void SetLogCallback(LOG_CALLBACK call) { callback_ = call; }


    void SetFilter(LogLevel filter) { filter_ = filter; }
    LogLevel GetFilter() const { return filter_; }

private:
    LOG_CALLBACK callback_;
    LogLevel filter_;
};

}