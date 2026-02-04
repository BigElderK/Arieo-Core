#include "base/prerequisites.h"
#include "core/core.h"
#include "core/config/config.h"
#include "core/module/module.h"
#include "core/singleton/singleton.h"

#include <filesystem>
#include <regex>
#include <string>


namespace Arieo::Core
{
    typedef void (*MODULE_ENTRY_FUN_PTR)(IProcessSingletonRegister*, Base::Memory::MemoryManager* defualt_memory_manager);
    typedef void (*MODULE_MAIN_FUN_PTR)();
    void ModuleManager::loadModuleLib(const std::filesystem::path& module_path, Base::Memory::MemoryManager* defualt_memory_manager)
    {
        SystemUtility::Lib::LIBTYPE module_lib = SystemUtility::Lib::loadLibrary(module_path.c_str());
        if(module_lib == nullptr)
        {
            Logger::error("Failed to load dll: {}, {}", module_path.string(), SystemUtility::Lib::getLastError());
            return;
        }

        MODULE_ENTRY_FUN_PTR module_entry_fun = (MODULE_ENTRY_FUN_PTR)SystemUtility::Lib::getProcAddress(module_lib, "ModuleEntry");
        if(module_entry_fun == nullptr)
        {
            Logger::fatal("Cannot found ModuleEntry for {}", module_path.string());
        }
        module_entry_fun(ProcessSingletonRegister::getProcessSingleton(), defualt_memory_manager);

        MODULE_MAIN_FUN_PTR module_main_fun = (MODULE_MAIN_FUN_PTR)SystemUtility::Lib::getProcAddress(module_lib, "ModuleMain");
        if(module_main_fun == nullptr)
        {
            Logger::error("Cannot found ModuleMain for {}", module_path.string());
            return;
        }
        module_main_fun();
    }
}