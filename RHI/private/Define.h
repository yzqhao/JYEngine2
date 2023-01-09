
#pragma once

#include "System/DataStruct.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef RHI_EXPORTS
    #define RHI_API     __declspec(dllexport)
#else
    #define RHI_API     __declspec(dllimport)
#endif
#else
    #define RHI_API
#endif

DECLARE_LOG_TAG(LogRHI)

NS_JYE_END
