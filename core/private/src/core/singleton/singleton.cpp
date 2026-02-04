#include "base/prerequisites.h"
#include "core/singleton/singleton.h"

#include <mutex>

namespace Arieo::Core
{
    static ProcessSingletonRegister s_process_singleton_register;
    static IProcessSingletonRegister* g_process_singleton_ptr = &s_process_singleton_register;
    static std::mutex mutex;

    void ProcessSingletonRegister::initializeFromModuleEntry(IProcessSingletonRegister* main_singleton_register)
    {
        g_process_singleton_ptr = main_singleton_register;
    }

    IProcessSingletonRegister* ProcessSingletonRegister::getProcessSingleton()
    {
        return g_process_singleton_ptr;
    }

    void ProcessSingletonRegister::registerProcessSingleton(size_t hash, void* instance)
    {
        std::lock_guard<std::mutex> lock(mutex);
        assert(m_instance_map.contains(hash));
        m_instance_map.emplace(hash, instance);
    }

    void* ProcessSingletonRegister::getRegisteredInstance(size_t hash)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto find_iter = m_instance_map.find(hash);
        if(find_iter == m_instance_map.end())
        {
            return nullptr;
        }
        return find_iter->second;
    }
}