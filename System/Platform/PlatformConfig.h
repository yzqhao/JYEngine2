
#pragma once

// define supported target platform macro which CC uses.
#define PLATFORM_UNKNOWN          

// Apple: Mac and iOS
#if __APPLE__ && !ANDROID // exclude android for binding generator.
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
        #define         PLATFORM_IOS 1
    #elif TARGET_OS_MAC
        #define         PLATFORM_MAC 1
    #endif
#endif

// android
#if ANDROID
    #define         PLATFORM_ANDROID 1
#endif

// win32
#if _WIN32 || _WINDOWS
    #define         PLATFORM_WIN32 1
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

#if PLATFORM_WIN32
#ifndef __MINGW32__
#pragma warning (disable:4127)
#endif
#if _WIN64
#define PLATFORM_64BITS 1
#else
#define PLATFORM_64BITS 0 
#endif
#else
#define PLATFORM_64BITS 1
#endif  // PLATFORM_WIN32

#if (PLATFORM_ANDROID) || (PLATFORM_IOS)
    #define PLATFORM_MOBILE
#else
    #define PLATFORM_PC
#endif

#if (PLATFORM_ANDROID) || (PLATFORM_IOS) || (PLATFORM_MAC)
    #define PLATFORM_UNIX
#endif

#if defined(__cplusplus) && ! defined(__APPLE__) && ! defined(ANDROID) && ! defined(LINUX)
#   define JY_EXTERN     extern "C"
#else
#   define JY_EXTERN		extern
#endif // __cplusplus

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <list>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <queue>
#include <array>

