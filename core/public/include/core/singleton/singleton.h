#pragma once
#include <mutex>
namespace Arieo::Core
{
    template<typename T>
    class ModuleSingleton
    {
    public:
        static T& getSingleton()
        {
            static T instance;
            return instance;
        }
    };

    class IProcessSingletonRegister
    {
    public:
        virtual void registerProcessSingleton(size_t hash, void* instance) = 0;
        virtual void* getRegisteredInstance(size_t hash) = 0;
    };

    class ProcessSingletonRegister
        : public IProcessSingletonRegister
    {
    public:
        static void initializeFromModuleEntry(IProcessSingletonRegister* main_singleton);
        static IProcessSingletonRegister* getProcessSingleton();

        void registerProcessSingleton(size_t hash, void* instance) override;
        void* getRegisteredInstance(size_t hash) override;
    protected:
        template<typename T, typename I>
        friend class ProcessSingleton;

        friend class ModuleManager;
        friend class PluginManager;

        std::unordered_map<size_t, void*> m_instance_map;
    };

    template<typename T, typename I>
    class ProcessSingleton
        : public I
    {
    public:
        static I& getProcessSingleton()
        {
            return getProcessSingleton_Internal();
        }
    protected:
        static T& getProcessSingleton_Internal()
        {
            static std::once_flag init_flag;
            static T* instance_ptr;
            std::call_once(init_flag, []() 
            {
                T* registered_instance_ptr = reinterpret_cast<T*>(ProcessSingletonRegister::getProcessSingleton()->getRegisteredInstance(Base::ct::genCrc32StringID(typeid(T).name())));    
                if(registered_instance_ptr == nullptr)
                {
                    static T new_instance;
                    instance_ptr = &new_instance;
                    ProcessSingletonRegister::getProcessSingleton()->registerProcessSingleton(Base::ct::genCrc32StringID(typeid(T).name()), instance_ptr);
                }
                else
                {
                    instance_ptr = registered_instance_ptr;
                }
                
            });

            assert(instance_ptr != nullptr);
            return *instance_ptr;
        }
    };
}