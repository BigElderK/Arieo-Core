#include "core/config/config.h"
#include "core/system/system_utility.h"
#include "core/logger/logger.h"

#include <fstream>
#include <sstream>

#include "core/manifest/manifest.h"

namespace Arieo::Core
{
    void Manifest::loadFromString(const std::string& manifest_content)
    {
        m_manifest_node = Core::ConfigFile::Load(manifest_content);
        m_host_os_name = Core::SystemUtility::getHostOSName();

        // Set environments from manifest
        m_system_node = m_manifest_node["app"]["host_os"][m_host_os_name];
        if (m_system_node.IsDefined() == false)
        {
            Logger::fatal("No configuration found for host OS: {}", m_host_os_name);
        }
    }

    void Manifest::loadFromFile(const std::filesystem::path& manifest_file_path)
    {
        std::ifstream manifest_file(manifest_file_path);
        if (!manifest_file.is_open())
        {
            Logger::fatal("Failed to open manifest file: {}", manifest_file_path.string());
        }

        std::stringstream buffer;
        buffer << manifest_file.rdbuf();
        manifest_file.close();

        loadFromString(buffer.str());
    }

    const std::filesystem::path Manifest::getEnginePath() const
    {
        if (!m_system_node.IsDefined())
        {
            Logger::fatal("No configuration found for host OS: {}", m_host_os_name);
        }

        Core::ConfigNode env_node = m_system_node["environments"]["ENGINE_INSTALL_PATH"];
        if (!env_node.IsDefined())
        {
            Logger::fatal("No ENGINE_INSTALL_PATH found for host OS: {}", m_host_os_name);
        }

        return Core::SystemUtility::FileSystem::getFormalizedPath(env_node.as<std::string>());
    }

    const std::vector<std::pair<std::string, std::string>> Manifest::getPresetEnvironments() const
    {
        std::vector<std::pair<std::string, std::string>> result;

        if (!m_system_node.IsDefined())
        {
            Logger::fatal("No configuration found for host OS: {}", m_host_os_name);
            return result;
        }

        Core::ConfigNode env_node = m_system_node["environments"];
        if (env_node.IsDefined())
        {
            for (auto env_iter = env_node.begin(); env_iter != env_node.end(); ++env_iter)
            {
                std::string env_name = env_iter->first.as<std::string>();
                std::string env_value;
                if (env_iter->second.IsSequence())
                {
                    // Join sequence values with ';'
                    std::stringstream ss;
                    for (auto value_iter = env_iter->second.begin(); value_iter != env_iter->second.end(); ++value_iter)
                    {
                        if (ss.tellp() > 0)
                        {
                            ss << ";";
                        }
                        ss << Core::SystemUtility::FileSystem::getFormalizedPath(value_iter->as<std::string>());
                    }
                    env_value = ss.str();
                }
                else
                {
                    env_value = Core::SystemUtility::FileSystem::getFormalizedPath(env_iter->second.as<std::string>());
                }
                result.emplace_back(env_name, env_value);
            }
        }

        return result;
    }

    void Manifest::applyPresetEnvironments() const
    {
        if (!m_system_node.IsDefined())
        {
            Logger::fatal("No configuration found for host OS: {}", m_host_os_name);
        }

        for (Core::ConfigNode::const_iterator env_node_iter = m_system_node["environments"].begin();
            env_node_iter != m_system_node["environments"].end();
            ++env_node_iter) 
        {
            std::string&& env_name = env_node_iter->first.as<std::string>();

            if(env_node_iter->second.IsSequence())
            {
                //If the value is in the seq mode, we append them to current environment.
                for (Core::ConfigNode::const_iterator env_value_iter = env_node_iter->second.begin();
                    env_value_iter != env_node_iter->second.end();
                    ++env_value_iter) 
                {
                    std::string&& append_env_value = env_value_iter->as<std::string>();
                    Core::SystemUtility::Environment::prependEnvironmentValue(
                        env_name, 
                        Core::SystemUtility::FileSystem::getFormalizedPath(append_env_value));
                }
            }
            else
            {
                //Replace the current environment
                std::string&& env_value = env_node_iter->second.as<std::string>();
                Core::SystemUtility::Environment::setEnvironmentValue(
                    env_name, 
                    Core::SystemUtility::FileSystem::getFormalizedPath(env_value));
            }
        }
    }

    const std::filesystem::path Manifest::getEngineModulePath(const std::string& module_name) const
    {
        auto all_modules = getAllEngineModulePaths();

        for (const auto& module_path : all_modules)
        {
            if (module_path.string().find(module_name) != std::string::npos)
            {
                return module_path;
            }
        }

        Logger::fatal("Module not found: {}", module_name);
        return std::filesystem::path();
    }

    const std::vector<std::filesystem::path> Manifest::getAllEngineModulePaths() const
    {
        std::vector<std::filesystem::path> result;

        if (!m_system_node.IsDefined())
        {
            Logger::fatal("No configuration found for host OS: {}", m_host_os_name);
        }

        // Search in modules list
        Core::ConfigNode modules_node = m_system_node["modules"];
        if (!modules_node.IsDefined() || !modules_node.IsSequence())
        {
            Logger::fatal("No modules list found for host OS: {}", m_host_os_name);
        }

        for (auto module_iter = modules_node.begin(); module_iter != modules_node.end(); ++module_iter)
        {
            std::string module_path = module_iter->as<std::string>();
            Logger::info("Found engine module in manifest: {}", module_path);
            result.emplace_back(Core::SystemUtility::FileSystem::getFormalizedPath(module_path));
        }

        return result;
    }

    Core::ConfigNode& Manifest::getSystemNode()
    {
        return m_system_node;
    }
}





