#pragma once
#include <string>
#include <filesystem>
#include "base/string/string_utility.h"
#include "core/config/config.h"

namespace Arieo::Core
{
    //Define in the base layer
    class Manifest
    {
    public:
        void loadFromString(const std::string& manifest_content);
        void loadFromFile(const std::filesystem::path& manifest_file_path);

        void applyPresetEnvironments() const;

        const std::filesystem::path getEnginePath() const;
        const std::filesystem::path getEngineModulePath(const std::string& module_name) const;

        const std::vector<std::pair<std::string, std::string>> getPresetEnvironments() const;
        const std::vector<std::filesystem::path> getAllEngineModulePaths() const;

        Core::ConfigNode& getSystemNode();
    private:
        Core::ConfigNode m_manifest_node;
        Core::ConfigNode m_system_node;
        std::string m_host_os_name;
    };
}




