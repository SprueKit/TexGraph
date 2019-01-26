#pragma once

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/MessageLog.h>

#ifndef SPRUELOG
    #define SPRUELOG 3
#endif

#if SPRUELOG >= 0
    #define SPRUE_LOG_ERROR(MSG) Context::GetInstance()->GetLog()->Log(MSG, LL_ERROR)
#else
    #define SPRUE_LOG_ERROR(MSG)
#endif

#if SPRUELOG >= 1
    #define SPRUE_LOG_WARNING(MSG) Context::GetInstance()->GetLog()->Log(MSG, LL_WARNING)
#else
    #define SPRUE_LOG_WARNING(MSG)
#endif

#if SPRUELOG >= 2
    #define SPRUE_LOG_INFO(MSG) Context::GetInstance()->GetLog()->Log(MSG, LL_INFO)
#else
    #define SPRUE_LOG_INFO(MSG)
#endif

#if SPRUELOG >= 3
    #define SPRUE_LOG_DEBUG(MSG) Context::GetInstance()->GetLog()->Log(MSG, LL_DEBUG)
#else
    #define SPRUE_LOG_DEBUG(MSG)
#endif