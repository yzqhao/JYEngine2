
#pragma once

#include "../../Core/Core.h"

NS_JYE_BEGIN

#ifdef _PLATFORM_WINDOWS
#ifdef IMGUI_FUNCTION_EXPORTS
    #define IMGUI_FUNCTION_API     __declspec(dllexport)
#else
    #define IMGUI_FUNCTION_API     __declspec(dllimport)
#endif
#else
    #define IMGUI_FUNCTION_API
#endif

NS_JYE_END
