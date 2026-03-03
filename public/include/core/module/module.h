#pragma once
#include "core/dymlib/dymlib.h"
#include "core/logger/logger.h"
#include "core/singleton/singleton.h"
#include "core/module/module_context.h"
#include "base/interop/interface.h"
#include "base/interop/instance.h"
#include "base/interop/interop.h"

#include <variant>
#include <functional>

namespace Arieo::Core
{
    class IModuleManager
    {
    public:
        virtual void loadModuleLib(const Base::Interop::StringView& module_path, Base::Memory::MemoryManager* default_memory_manager) = 0;
    protected:
        friend class ModuleManager;
        virtual void registerInterface(const std::type_info& type_info, const Base::Interop::StringView& instance_alias_name, Base::Interop::SharedRef<void> instance) = 0;
        virtual void unregisterInterface(const std::type_info& type_info, const Base::Interop::SharedRef<void>& interface) = 0;
        virtual Base::Interop::SharedRef<void> getInterface(std::size_t type_hash, const Base::Interop::StringView& instance_alias_name) = 0;
    };

    class ModuleManager
        : public ProcessSingleton<ModuleManager, IModuleManager>
    {
    public:
        template<class TInterface>
        static void registerInterface(const Base::Interop::StringView& instance_name, Base::Interop::SharedRef<TInterface> interface)
        {
            const std::type_info& type_info_of_t = typeid(TInterface);
            return getProcessSingleton().registerInterface(type_info_of_t, instance_name.getStringView(), Base::Interop::SharedRef<void>(interface));
        }

        template<class TInterface>
        static void unregisterInterface(const Base::Interop::SharedRef<TInterface>& interface)
        {
            // TODO: implement it
            return getProcessSingleton().unregisterInterface(typeid(TInterface), Base::Interop::SharedRef<void>(interface));
        }

        template<class T>
        static Base::Interop::SharedRef<T> getInterface(const Base::Interop::StringView& instance_name = "")
        {
            const std::type_info& type_info_of_t = typeid(T);
            std::size_t type_hash = Base::ct::genCrc32StringID(type_info_of_t.name());

            return getProcessSingleton().getInterface(type_hash, instance_name.getStringView()).queryInterfaceForcely<T>();
        }

        void loadModuleLib(const Base::Interop::StringView& module_path, Base::Memory::MemoryManager* default_memory_manager) override;
    protected:
        friend class MainModule;

        Base::Interop::SharedRef<void> getInterface(std::size_t type_hash, const Base::Interop::StringView& instance_alias_name) override
        {
            auto iter = m_registered_interfaces.find(type_hash);
            if(iter == m_registered_interfaces.end())
            {
                Core::Logger::error("Cannot found interface with hash {}", type_hash);
                return nullptr;
            }

            const auto name_str = instance_alias_name.getString();
            if(name_str.empty())
            {
                if(iter->second.size() > 0)
                {
                    return iter->second.begin()->second;
                }
            }
            else
            {
                auto iter_2 = iter->second.find(name_str);
                if(iter_2 != iter->second.end())
                {
                    return iter_2->second;
                }
            }

            return nullptr;
        }

        void registerInterface(const std::type_info& type_info, const Base::Interop::StringView& instance_alias_name, Base::Interop::SharedRef<void> instance) override
        {
            std::size_t type_hash = Base::ct::genCrc32StringID(type_info.name());
            
            auto iter = m_registered_interfaces.find(type_hash);
            if(iter == m_registered_interfaces.end())
            {
                m_registered_interfaces.emplace(type_hash, std::unordered_map<std::string, Base::Interop::SharedRef<void>>());
                iter = m_registered_interfaces.find(type_hash);
            }
            
            const auto name_str = instance_alias_name.getString();
            iter->second.emplace(name_str, instance);
            Core::Logger::trace("Interface registered: interface {} of {} with hash {}", type_info.name(), name_str, type_hash);
        }

        void unregisterInterface(const std::type_info& type_info, const Base::Interop::SharedRef<void>& interface) override
        {
            std::size_t type_hash = Base::ct::genCrc32StringID(type_info.name());
            
            auto iter = m_registered_interfaces.find(type_hash);
            if(iter == m_registered_interfaces.end())
            {
                Core::Logger::error("Cannot found interface with hash {} when unregistering", type_hash);
                return;
            }
            
            // Search by value to find the interface to unregister
            auto iter_2 = std::find_if(iter->second.begin(), iter->second.end(),
                [&interface](const auto& pair) { return pair.second == interface; });
            if(iter_2 != iter->second.end())
            {
                iter->second.erase(iter_2);
                Core::Logger::trace("Interface unregistered: interface {} with hash {}", type_info.name(), type_hash);
            }
        }

        std::unordered_map<std::size_t, std::unordered_map<std::string, Base::Interop::SharedRef<void>>> m_registered_interfaces;
    };
    
    #define GENERATOR_MODULE_ENTRY_FUN() \
    ARIEO_DLLEXPORT void ModuleEntry(Arieo::Core::IProcessSingletonRegister* main_process_singleton_register, Base::Memory::MemoryManager* default_memory_manager)\
    {\
        Arieo::Core::ProcessSingletonRegister::initializeFromModuleEntry(main_process_singleton_register);\
        Base::Memory::MemoryManager::initialize(default_memory_manager);\
    }
}





