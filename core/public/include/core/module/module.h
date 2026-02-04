#pragma once
#include "core/dymlib/dymlib.h"
#include "core/logger/logger.h"
#include "core/singleton/singleton.h"
#include "core/module/module_context.h"

#include <filesystem>
#include <variant>
#include <functional>

namespace Arieo::Core
{
    class IModuleManager
    {
    public:
        virtual void loadModuleLib(const std::filesystem::path& module_path, Base::Memory::MemoryManager* defualt_memory_manager) = 0;
    protected:
        friend class ModuleManager;
        virtual void registerInterface(const std::type_info& type_info, const std::string& instance_alias_name, void* instance) = 0;
        virtual void* getInterface(std::size_t type_hash, const std::string& instance_alias_name) = 0;
    };

    class ModuleManager
        : public ProcessSingleton<ModuleManager, IModuleManager>
    {
    public:
        template<class T>
        static void registerInterface(std::string instance_name, T* module_interface)
        {
            const std::type_info& type_info_of_t = typeid(T);
            return getProcessSingleton().registerInterface(type_info_of_t, instance_name, module_interface);
        }

        template<class T>
        static void unregisterInterface(T* module_interface)
        {
            //TODO: implement it
        }

        template<class T>
        static T* getInterface(const std::string& instance_name = "")
        {
            const std::type_info& type_info_of_t = typeid(T);
            std::size_t type_hash = Base::ct::genCrc32StringID(type_info_of_t.name());

            return reinterpret_cast<T*>(
                getProcessSingleton().getInterface(type_hash, instance_name)
            );
        }

        static void* getInterfaceRaw(std::size_t type_hash, const std::string& instance_name = "")
        {
            return getProcessSingleton().getInterface(type_hash, instance_name);
        }

        void loadModuleLib(const std::filesystem::path& module_path, Base::Memory::MemoryManager* defualt_memory_manager) override;
    protected:
        friend class MainModule;

        void* getInterface(std::size_t type_hash, const std::string& instance_alias_name) override
        {
            auto iter = m_registered_interfaces.find(type_hash);
            if(iter == m_registered_interfaces.end())
            {
                Core::Logger::error("Cannot found interface with hash {}", type_hash);
                return nullptr;
            }

            if(instance_alias_name.length() == 0 || instance_alias_name == "")
            {
                if(iter->second.size() > 0)
                {
                    return iter->second.begin()->second;
                }
            }
            else
            {
                auto iter_2 = iter->second.find(instance_alias_name);
                if(iter_2 != iter->second.end())
                {
                    return iter_2->second;
                }
            }

            return nullptr;
        }

        void registerInterface(const std::type_info& type_info, const std::string& instance_alias_name, void* instance) override
        {
            std::size_t type_hash = Base::ct::genCrc32StringID(type_info.name());
            
            auto iter = m_registered_interfaces.find(type_hash);
            if(iter == m_registered_interfaces.end())
            {
                m_registered_interfaces.emplace(type_hash, std::move(std::unordered_map<std::string, void*>()));
                iter = m_registered_interfaces.find(type_hash);
            }
            
            iter->second.emplace(instance_alias_name, instance);
            Core::Logger::trace("Interface registered: interface {} of {} with hash {}", type_info.name(), instance_alias_name, type_hash);
        }

        std::unordered_map<std::size_t, std::unordered_map<std::string, void*>> m_registered_interfaces;

    };
    
    #define GENERATOR_MODULE_ENTRY_FUN() \
    ARIEO_DLLEXPORT void ModuleEntry(Arieo::Core::IProcessSingletonRegister* main_process_singleton_register, Base::Memory::MemoryManager* defualt_memory_manager)\
    {\
        Arieo::Core::ProcessSingletonRegister::initializeFromModuleEntry(main_process_singleton_register);\
        Base::Memory::MemoryManager::initialize(defualt_memory_manager);\
    }
}
