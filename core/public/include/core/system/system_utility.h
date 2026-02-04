#pragma once
#include "base/prerequisites.h"
#include <string>

namespace Arieo::Core
{
    class SystemUtility
    {
    public:
        class Environment
        {
        public:
            static void setEnvironmentValue(const std::string& name, const std::string& value);
            static void prependEnvironmentValue(const std::string& name, const std::string& value);
            static void appendEnvironmentValue(const std::string& name, const std::string& value);
            static std::string getEnvironmentValue(const std::string& name);
        };

        class FileSystem
        {
        public:
            static void listFilesRecursively(const std::filesystem::path& dir_path, const std::regex& pattern, std::vector<std::filesystem::path>& result);
            static void listFiles(const std::filesystem::path& dir_path, const std::regex& pattern, std::vector<std::filesystem::path>& result);

            static std::filesystem::path getExePath();
            static std::filesystem::path getModulePath();
            static std::filesystem::path getCurrentPath();
            static std::string getFormalizedPath(const std::string& formalized_path);
        };

        class Lib
        {
        public:
            using LIBTYPE = void*;
            static LIBTYPE loadLibrary(const std::filesystem::path& lib_file_path);
            static void* getProcAddress(LIBTYPE lib, const std::string& function_name);
            static const char* getDymLibFilePattern();
            static const char* getDymLibFileExtension();
            static const char* getDymLibFilePrefix();
            static const char* getLastError();
            static std::string getDymLibFileName(std::string lib_name);
        };

        static std::string getPlatformName();
        static std::string getHostOSName();
    };
}