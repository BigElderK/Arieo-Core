#pragma once
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <filesystem>
#include <regex>

#define __INTRIN_H
#include <fmt/format.h>
#include <fmt/base.h>

namespace Arieo::Base
{
    namespace StringUtility
    {
        void replaceAll(std::string& src, const std::string& replace_sub_str, const std::string& replace_to);
        std::wstring getWString(const std::string& source);

        template <typename... T>
        ARIEO_INLINE auto format(fmt::v12::format_string<T...> fmt, T&&... args)
            -> std::string {
          return vformat(fmt.str, fmt::v12::vargs<T...>{{args...}});
        }

        std::string toLower(const std::string& str);

        template<typename T>
        class TCharStringCoverter
        {
        public:
            static const std::string& getString(const std::string& src)
            {
                return src;
            }
        };

        template<>
        class TCharStringCoverter<wchar_t>
        {
        public:
            static const std::wstring getString(const std::string& src)
            {
                return getWString(src);
            }
        };
    };
}

// Add fmt formatter for std::filesystem::path
template <>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<std::string> {
    auto format(const std::filesystem::path& path, format_context& ctx) const {
        return fmt::formatter<std::string>::format(path.string(), ctx);
    }
};