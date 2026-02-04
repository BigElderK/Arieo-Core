#pragma once
#include <string>
#include "base/string/string_utility.h"
namespace Arieo::Core
{
    //Define in the base layer
    //Implement in the platform layer
    class Logger
    {
    public:
        static void setDefaultLogger(const std::string&& logger_name);

        static void trace(const std::string& expression);
        static void debug(const std::string& expression);
        static void info(const std::string& expression);
        static void warn(const std::string& expression);
        static void error(const std::string& expression);
        static void fatal(const std::string& expression);
        static void verbose(const std::uint8_t level, const std::string& expression);

        template<typename... Args>
        ARIEO_INLINE static void trace(fmt::v12::format_string<Args...>&& expression, Args&&... args)
        {
            return trace(Base::StringUtility::format(expression, std::forward<Args>(args)...));
        }

        template<typename... Args>
        ARIEO_INLINE static void debug(fmt::v12::format_string<Args...>&& expression, Args&&... args)
        {
            return debug(Base::StringUtility::format(expression, std::forward<Args>(args)...));
        }

        template<typename... Args>
        ARIEO_INLINE static void info(fmt::v12::format_string<Args...>&& expression, Args&&... args)
        {
            return info(Base::StringUtility::format(expression, std::forward<Args>(args)...));
        }

        template<typename... Args>
        ARIEO_INLINE static void warn(fmt::v12::format_string<Args...>&& expression, Args&&... args)
        {
            return warn(Base::StringUtility::format(expression, std::forward<Args>(args)...));
        }

        template<typename... Args>
        ARIEO_INLINE static void error(fmt::v12::format_string<Args...>&& expression, Args&&... args)
        {
            return error(Base::StringUtility::format(expression, std::forward<Args>(args)...));
        }

        template<typename... Args>
        ARIEO_INLINE static void fatal(fmt::v12::format_string<Args...>&& expression, Args&&... args)
        {
            return fatal(Base::StringUtility::format(expression, std::forward<Args>(args)...));
        }

        template<typename... Args>
        ARIEO_INLINE static void verbose(const std::uint8_t level, fmt::v12::format_string<Args...>&& expression, Args&&... args)
        {
            return verbose(level, Base::StringUtility::format(expression, std::forward<Args>(args)...));
        }
    };
    
    #define ARIEO_ASSERT(condition) if(!(condition)){Arieo::Core::Logger::fatal(#condition);}
}