
#include "base/prerequisites.h"
#include "core/core.h"

#if defined(ARIEO_PLATFORM_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#include <dlfcn.h>
#include <limits.h>
#include <regex>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <unistd.h>

namespace Arieo::Core
{
    std::string SystemUtility::getPlatformName()
    {
        return "emscripten";
    }
}

namespace Arieo::Core
{
    void SystemUtility::Environment::setEnvironmentValue(const std::string& name, const std::string& value)
    {
        ARIEO_ASSERT(setenv(name.c_str(), value.c_str(), 1) == 0);
    }

    std::string SystemUtility::Environment::getEnvironmentValue(const std::string& name)
    {
        const char* value = getenv(name.c_str());
        if(value == nullptr)
        {
            return std::string("");
        }
        return std::string(value);
    }

    void SystemUtility::Environment::prependEnvironmentValue(const std::string& name, const std::string& value)
    {
        std::string&& original_value = getEnvironmentValue(name);
        std::string new_value;

        if(original_value.empty())
        {
            new_value = value;
        }
        else
        {
            new_value = value + ":" + original_value;
        }

        setEnvironmentValue(name, new_value);
    }

    void SystemUtility::Environment::appendEnvironmentValue(const std::string& name, const std::string& value)
    {
        std::string&& original_value = getEnvironmentValue(name);
        std::string new_value;

        if(original_value.empty())
        {
            new_value = value;
        }
        else
        {
            new_value = original_value + ":" + value;
        }
        setEnvironmentValue(name, new_value);
    }
}

namespace Arieo::Core
{
    std::filesystem::path SystemUtility::FileSystem::getExePath()
    {
        char buf[PATH_MAX];
        if(getcwd(buf, PATH_MAX) != nullptr)
        {
            return std::filesystem::path(buf);
        }
        return std::filesystem::path("/");
    }

    std::filesystem::path SystemUtility::FileSystem::getModulePath()
    {
        Dl_info dlInfo;
        dladdr((const void*)SystemUtility::FileSystem::getModulePath, &dlInfo);

        if(dlInfo.dli_fname == nullptr)
        {
            Logger::trace("It's strange but call SystemUtility::FileSystem::getModulePath failed");
            return std::filesystem::path("/");
        }

        return dlInfo.dli_fname;
    }
}

namespace Arieo::Core
{
    SystemUtility::Lib::LIBTYPE SystemUtility::Lib::loadLibrary(const std::filesystem::path& lib_file_path)
    {
        return dlopen(lib_file_path.string().c_str(), RTLD_LAZY);
    }

    void* SystemUtility::Lib::getProcAddress(LIBTYPE lib, const std::string& function_name)
    {
        return dlsym((void*)lib, function_name.c_str());
    }

    const char* SystemUtility::Lib::getDymLibFilePattern()
    {
        return "(.*)(.wasm)";
    }

    const char* SystemUtility::Lib::getDymLibFileExtension()
    {
        return "wasm";
    }

    const char* SystemUtility::Lib::getDymLibFilePrefix()
    {
        return "";
    }

    const char* SystemUtility::Lib::getLastError()
    {
        return dlerror();
    }
}
#endif




