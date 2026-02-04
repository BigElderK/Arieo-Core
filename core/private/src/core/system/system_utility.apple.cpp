
#include "base/prerequisites.h"
#include "core/core.h"

#if defined(ARIEO_PLATFORM_APPLE)
#include <iostream>
#include <regex>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>

namespace Arieo::Core
{
    std::string SystemUtility::getPlatformName()
    {
        return "apple";
    }
}
namespace Arieo::Core
{
    void SystemUtility::Environment::setEnvironmentValue(const std::string& name, const std::string& value)
    {
       //_putenv_s(name.c_str(), value.c_str());
       //std::string formated_str = StringUtility::format("{}={}", name, value);
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
            new_value = original_value;
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
            new_value = original_value;
        }
        else
        {
            new_value = value + ":" + original_value;
        }
        setEnvironmentValue(name, new_value);
    }
}

#include <mach-o/dyld.h>
#include <limits.h>
namespace Arieo::Core
{
    std::filesystem::path SystemUtility::FileSystem::getExePath()
    {
        char path[PATH_MAX];
        uint32_t size_of_path = sizeof(path);
        if (_NSGetExecutablePath(path, &size_of_path) == 0) 
        {
            return std::string((const char*)path);
        } 
        else 
        {
            // Buffer too small, need to allocate larger buffer
            char* buffer = (char*)malloc(size_of_path);
            _NSGetExecutablePath(buffer, &size_of_path);
            std::string result = std::string(buffer);
            free(buffer);
            return result;
        }
    }
}

namespace Arieo::Core
{
    std::filesystem::path SystemUtility::FileSystem::getModulePath()
    {
        //https://stackoverflow.com/questions/43607115/how-to-get-current-library-path
        //Tobe implement
        //Dl_info dlInfo;
        //dladdr(puts, &dlInfo);
        Dl_info dlInfo;
        dladdr((const void*)SystemUtility::FileSystem::getModulePath, &dlInfo);

        if (dlInfo.dli_fname == nullptr)
        {
            Logger::trace("It's strange but call SystemUtility::FileSystem::getModulePath failed");
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
        return "(.*)(.so)";
    }

    const char* SystemUtility::Lib::getDymLibFileExtension()
    {
        return "so";
    }

    const char* SystemUtility::Lib::getDymLibFilePrefix()
    {
        return "lib";
    }   

    const char* SystemUtility::Lib::getLastError()
    {
        return dlerror();
    }
}

#endif