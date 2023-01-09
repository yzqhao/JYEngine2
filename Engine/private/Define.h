
#pragma once

#include "../../Core/Core.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef ENGINE_EXPORTS
    #define ENGINE_API     __declspec(dllexport)
#else
    #define ENGINE_API     __declspec(dllimport)
#endif
#else
    #define ENGINE_API
#endif

DECLARE_LOG_TAG(LogEngineRHI)

NS_JYE_END
