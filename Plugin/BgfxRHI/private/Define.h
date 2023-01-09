
#pragma once

#include "System/DataStruct.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef BGFX_RHI_EXPORTS
    #define BGFXRHI_API     __declspec(dllexport)
#else
    #define BGFXRHI_API     __declspec(dllimport)
#endif
#else
    #define BGFXRHI_API
#endif

DECLARE_LOG_TAG(LogBgfxRHI)

NS_JYE_END
