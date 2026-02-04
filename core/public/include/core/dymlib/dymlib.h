#pragma once
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <filesystem>
#include <regex>

namespace Arieo::Core
{
#ifdef ARIEO_PLATFORM_WINDOWS
    #define ARIEO_DLLEXPORT extern "C" __declspec(dllexport)
#else
    #define ARIEO_DLLEXPORT extern "C"
#endif


#ifdef ARIEO_PLATFORM_WINDOWS
    #ifdef ARIEO_DLL_EXPORTS
        #define ARIEO_CLASS_API __declspec(dllexport)
    #else
        #define ARIEO_CLASS_API __declspec(dllimport)
    #endif
#else
    #define ARIEO_CLASS_API __attribute__((visibility("default")))
#endif
}