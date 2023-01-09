
#pragma once

#include "../../System/System.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef RENDER_PIPELINE_SYSTEM_EXPORTS
    #define RENDER_PIPELINE_SYSTEM_API     __declspec(dllexport)
#else
    #define RENDER_PIPELINE_SYSTEM_API     __declspec(dllimport)
#endif
#else
    #define RENDER_PIPELINE_SYSTEM_API
#endif

NS_JYE_END
