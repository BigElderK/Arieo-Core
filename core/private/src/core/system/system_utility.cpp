
#include "base/prerequisites.h"
#include "core/core.h"

namespace Arieo::Core
{
    std::string SystemUtility::getHostOSName()
    {
        return ARIEO_HOST_OS;
    }

    std::filesystem::path SystemUtility::FileSystem::getCurrentPath()
    {
        return std::filesystem::current_path();
    }

    void SystemUtility::FileSystem::listFiles(const std::filesystem::path& dir_path, const std::regex& pattern, std::vector<std::filesystem::path>& result)
    {
        //string strPath = "D:\\pic\\new";
        //regex fileSuffix("(.*)(.jpg)");// *.jpg, *.png  
        //regex fileSuffix("(.*).(.jpg)");
        //regex fileSuffix(".*z.*\\.(jpg|png)");//all jpg or png contains letter 'Z'
        for (auto& DirectoryIter : std::filesystem::directory_iterator(dir_path))
        {
            auto filepath = DirectoryIter.path();
            auto filename = filepath.filename();

            if (std::regex_match(filename.string(), pattern))
            {
                result.push_back(filepath.string());
            }
        }
    }

    void SystemUtility::FileSystem::listFilesRecursively(const std::filesystem::path& dir_path, const std::regex& pattern, std::vector<std::filesystem::path>& result)
    {
        //string strPath = "D:\\pic\\new";
        //regex fileSuffix("(.*)(.jpg)");// *.jpg, *.png  
        //regex fileSuffix("(.*).(.jpg)");
        //regex fileSuffix(".*z.*\\.(jpg|png)");//all jpg or png contains letter 'Z'
        
        if(std::filesystem::exists(dir_path) == false)
        {
            return;
        }

        for (auto& DirectoryIter : std::filesystem::recursive_directory_iterator(dir_path))
        {
            auto filepath = DirectoryIter.path();
            auto filename = filepath.filename();

            if (std::regex_match(filename.string(), pattern))
            {
                result.push_back(filepath.string());
            }
        }
    }

    std::string SystemUtility::FileSystem::getFormalizedPath(const std::string& formalized_path)
    {
        std::string result_path = formalized_path;
        std::filesystem::path module_path = std::filesystem::absolute(getModulePath().parent_path());
        std::filesystem::path exe_path = std::filesystem::absolute(getExePath().parent_path());

        Base::StringUtility::replaceAll(result_path, "${EXE_DIR}", exe_path.string());
        Base::StringUtility::replaceAll(result_path, "${MODULE_DIR}", module_path.string());
        Base::StringUtility::replaceAll(result_path, "$ENV{APP_INTERNAL_DATA_DIR}", SystemUtility::Environment::getEnvironmentValue("APP_INTERNAL_DATA_DIR"));
        Base::StringUtility::replaceAll(result_path, "$ENV{APP_EXTERNAL_DATA_DIR}", SystemUtility::Environment::getEnvironmentValue("APP_EXTERNAL_DATA_DIR"));

        #define STRINGIFY2(X) #X
        #define STRINGIFY(X) STRINGIFY2(X)
        Base::StringUtility::replaceAll(result_path, "${CMAKE_BUILD_TYPE}", STRINGIFY(CMAKE_BUILD_TYPE));
        Base::StringUtility::replaceAll(result_path, "${CMAKE_HOST_SYSTEM_NAME}", STRINGIFY(CMAKE_HOST_SYSTEM_NAME));
        Base::StringUtility::replaceAll(result_path, "${CMAKE_HOST_SYSTEM_PROCESSOR}", STRINGIFY(CMAKE_HOST_SYSTEM_PROCESSOR));

        Base::StringUtility::replaceAll(result_path, "${MODULE_FILE_EXTENSION}", SystemUtility::Lib::getDymLibFileExtension());
        Base::StringUtility::replaceAll(result_path, "${DYM_LIB_FILE_PREFIX}", SystemUtility::Lib::getDymLibFilePrefix());
        Base::StringUtility::replaceAll(result_path, "${DYM_LIB_FILE_EXTENSION}", SystemUtility::Lib::getDymLibFileExtension());

        Base::StringUtility::replaceAll(result_path, "\\", "/");

#if defined(ARIEO_PLATFORM_WINDOWS)
            // On Windows, convert to lowercase to avoid case sensitivity issues
            Base::StringUtility::replaceAll(result_path, "\\", "/");
#endif
        return result_path;
    }

    std::string SystemUtility::Lib::getDymLibFileName(std::string lib_name)
    {
        return Arieo::Base::StringUtility::format(
            "{}{}.{}",
            Arieo::Core::SystemUtility::Lib::getDymLibFilePrefix(),
            lib_name,
            Arieo::Core::SystemUtility::Lib::getDymLibFileExtension()
        );
    }
}