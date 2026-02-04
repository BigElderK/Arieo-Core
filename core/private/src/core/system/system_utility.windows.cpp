
#include "base/prerequisites.h"
#include "core/core.h"

#if defined(ARIEO_PLATFORM_WINDOWS)
#include <windows.h>
#include <regex>
#include <stdlib.h>
#include <string>
namespace Arieo::Core
{
    std::string SystemUtility::getPlatformName()
    {
        return "windows";
    }
}
namespace Arieo::Core
{
    void SystemUtility::Environment::setEnvironmentValue(const std::string& name, const std::string& value)
    {
        ARIEO_ASSERT(_putenv_s(name.c_str(), value.c_str()) == 0);
    }

    std::string SystemUtility::Environment::getEnvironmentValue(const std::string& name)
    {
        char* buf = nullptr;
        size_t size = 0;
        if(_dupenv_s(&buf, &size, name.c_str()) != 0
            || buf == nullptr)
        {
            return std::string();
        }

        std::string ret_string(buf);
        free(buf);
        
        return ret_string;
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
            new_value = value + ";" + original_value;
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
            new_value = value + ";" + original_value;
        }
        setEnvironmentValue(name, new_value);
    }

}

namespace Arieo::Core
{
    std::filesystem::path SystemUtility::FileSystem::getExePath()
    {
        char file_path_str[MAX_PATH];
        MEMORY_BASIC_INFORMATION mbi;  
        ::GetModuleFileNameA(
                nullptr, 
                file_path_str,
                MAX_PATH
        );

        std::filesystem::path file_path = std::filesystem::path(file_path_str);
        return file_path;
    }

    std::filesystem::path SystemUtility::FileSystem::getModulePath()
    {
        char file_path_str[MAX_PATH];

        MEMORY_BASIC_INFORMATION mbi;  
        
        HMODULE current_module = ((::VirtualQuery(
                reinterpret_cast<void*>(getModulePath), 
                &mbi, sizeof(mbi)) != 0)  
            ? (HMODULE) mbi.AllocationBase : NULL);  

        ::GetModuleFileNameA(
                current_module, 
                file_path_str,
                MAX_PATH
        );

        std::filesystem::path file_path = std::filesystem::path(file_path_str);
        return file_path;
    }
}

namespace Arieo::Core
{
    SystemUtility::Lib::LIBTYPE SystemUtility::Lib::loadLibrary(const std::filesystem::path& lib_file_path)
    {
        SystemUtility::Lib::LIBTYPE lib = ::LoadLibraryA(lib_file_path.string().c_str());
        if(lib == nullptr)
        {
            // covert error code to string
            DWORD error_code = ::GetLastError();
            printf("LoadLibraryA failed with error code: %lu\r\n", error_code);
            return nullptr;
        }
        return lib;
    }

    void* SystemUtility::Lib::getProcAddress(LIBTYPE lib, const std::string& function_name)
    {
        return reinterpret_cast<void*>(::GetProcAddress((HMODULE)lib, function_name.c_str()));
    }

    const char* SystemUtility::Lib::getDymLibFilePattern()
    {
        return "(.*)(.dll)";
    }

    const char* SystemUtility::Lib::getDymLibFileExtension()
    {
        return "dll";
    }

    const char* SystemUtility::Lib::getDymLibFilePrefix()
    {
        return "";
    }   

    const char* SystemUtility::Lib::getLastError()
    {
        return "Unknown";
    }
}
#endif