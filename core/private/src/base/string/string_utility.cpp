#include "base/prerequisites.h"
#include "base/string/string_utility.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace Arieo::Base
{
    void StringUtility::replaceAll(std::string& src, const std::string& replace_sub_str, const std::string& replace_to)
    {
        size_t pos;
        while ((pos = src.find(replace_sub_str)) != std::string::npos) 
        {
            src.replace(pos, replace_sub_str.length(), replace_to);
        }
        return;
    }

    std::wstring StringUtility::getWString(const std::string& source)
    {
        return std::wstring(source.begin(), source.end());
    }

    std::string StringUtility::toLower(const std::string& str)
    {
        std::string ret(str);
        std::transform(ret.begin(), ret.end(), ret.begin(), 
            [](unsigned char c){ return std::tolower(c); });
        return ret;
    }

}