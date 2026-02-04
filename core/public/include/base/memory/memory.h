#pragma once

#include <cstddef>
#include <memory>
#include <memory_resource>
#include <cstddef>
#include <cstdlib>
namespace Arieo::Base::Memory
{
    class IAllocator
    {
    public:
        virtual void* allocate(size_t bytes, size_t alignment) = 0;
        virtual void deallocate(void* p, size_t bytes, size_t alignment) = 0;
    };

    class MemoryManager
    {
    public:
        static void initialize(MemoryManager* process_singleton);
        static MemoryManager m_singleton;
    public:
        IAllocator* m_default_memory_allocator;
        IAllocator* m_resource_memory_allocator;
        IAllocator* m_runtime_memory_allocator;
        IAllocator* m_frame_memory_allocator;
        IAllocator* m_stack_memory_allocator;
    };
}

namespace Arieo::Base::Memory
{ 
    inline void* malloc(size_t size)
    {
        return MemoryManager::m_singleton.m_default_memory_allocator->allocate(size, sizeof(double));
    }

    inline void free(void* ptr)
    {
        // TODO: 0, 0
        return MemoryManager::m_singleton.m_default_memory_allocator->deallocate(ptr, 0, 0);
    }
}

namespace Arieo::Base
{
    template <typename T, typename... Args>
    inline T* newT(Args&&... args) 
    {
        return new (Arieo::Base::Memory::malloc(sizeof(T))) T(std::forward<Args>(args)...);
    }

    template <typename T>
    inline void deleteT(T* instance) 
    {
        instance->~T();
        Arieo::Base::Memory::free(instance);
    }
}

