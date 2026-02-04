#pragma once

#undef _DEBUG_ENABLE_EQUIVAL_TYPE_CHECK

#ifdef _DEBUG
    #define _DEBUG_ENABLE_EQUIVAL_TYPE_CHECK
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define ARIEO_PLATFORM_WINDOWS
    #define ARIEO_PLATFORM_NAME "Windows"
#elif defined(__ANDROID__)
    #define ARIEO_PLATFORM_ANDROID
    #define ARIEO_PLATFORM_NAME "Android"
#elif defined(__linux__)
    #define ARIEO_PLATFORM_LINUX
    #define ARIEO_PLATFORM_NAME "Linux"
#elif defined(__APPLE__)
    #define ARIEO_PLATFORM_APPLE
    #define ARIEO_PLATFORM_NAME "Apple"
#else
    #define ARIEO_PLATFORM_NAME "Unknown"
#endif

#ifdef _DEBUG
    #define ARIEO_ALWAYS_INLINE __attribute__((always_inline))
    #define ARIEO_INLINE ARIEO_ALWAYS_INLINE
#else
    #define ARIEO_INLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define ARIEO_DISABLE_CLANG_WARNING(warning) \
        _Pragma("GCC diagnostic push") \
        _Pragma(##warning)
    #define ARIEO_ENABLE_CLANG_WARNING()\
        _Pragma("GCC diagnostic pop")
    
    #define ARIEO_DISABLE_MSVC_WARNING(warning)
    #define ARIEO_ENABLE_MSVC_WARNING()
#elif defined(_MSC_VER)
    #define ARIEO_DISABLE_CLANG_WARNING(warning)
    #define ARIEO_ENABLE_CLANG_WARNING()
    
    #define ARIEO_DISABLE_MSVC_WARNING(warning) \
        __pragma(warning(push)) \
        __pragma(warning(disable: warning))
    #define ENABLE_MSVC_WARNING()
        __pragma(warning(pop))
#else
    #define ARIEO_DISABLE_CLANG_WARNING(warning)
    #define ARIEO_ENABLE_CLANG_WARNING()
    #define ARIEO_DISABLE_MSVC_WARNING(warning)
    #define ARIEO_ENABLE_MSVC_WARNING()
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define ARIEO_CLANG_PRAGMA _Pragma
    #define ARIEO_MSVC_PRAGMA(p)
#elif defined(_MSC_VER)
    #define ARIEO_CLANG_PRAGMA(p)
    #define ARIEO_MSVC_PRAGMA __pragma
#else
    #define ARIEO_CLANG_PRAGMA
    #define ARIEO_MSVC_PRAGMA
#endif