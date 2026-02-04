#include "base/prerequisites.h"

#include <memory>
#include <memory_resource>

namespace Arieo::Base::Memory
{
    // class MimallocResource : public std::pmr::memory_resource 
    // {
    // private:
    //     mi_heap_t* m_mi_heap = nullptr;
    // public:
    //     MimallocResource()
    //     {
    //         m_mi_heap = mi_heap_new();
    //     }   
        
    //     ~MimallocResource()
    //     {
    //         mi_heap_destroy(m_mi_heap);
    //     }
        
    //     void* do_allocate(size_t bytes, size_t alignment) override 
    //     {
    //         // std::cout << "Allocating " << bytes << " bytes\n";
    //         // return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    //         return mi_heap_malloc_aligned(m_mi_heap, bytes, alignment);
    //     }
        
    //     void do_deallocate(void* p, size_t bytes, size_t alignment) override 
    //     {
    //         // std::cout << "Deallocating " << bytes << " bytes\n";
    //         // return std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    //         mi_free(p);
    //     }
        
    //     bool do_is_equal(const memory_resource& other) const noexcept override 
    //     {
    //         return this == &other;
    //     }
    // };

    // // For each module, have a single instance of it.
    // MimallocResource m_mi_default_mem_resource;
    // MimallocResource m_mi_resource_mem_resource;
    // MimallocResource m_mi_runtime_mem_resource;
    // MimallocResource m_mi_frame_mem_resource;
    // MimallocResource m_mi_stack_mem_resource;

    // class MemoryInitializer
    // {
    // public:
    //     MemoryInitializer()
    //     {
    //         Memory::initialize();
    //     }
    // };
    // static MemoryInitializer g_memory_initializer;

    MemoryManager MemoryManager::m_singleton;
    void MemoryManager::initialize(MemoryManager* process_singleton)
    {
        m_singleton = *process_singleton;
    }

    // void Memory::initialize()
    // {
        //Core::Logger::trace("pmr memory init ...");

        // mi_option_enable(mi_option_show_errors);  // show runtime errors
        // mi_option_enable(mi_option_verbose);      // verbose output

        //TOOD: load memory limits from config file. 
        // g_default_memory = &m_mi_default_mem_resource;//std::pmr::get_default_resource();
        // g_resource_memory = &m_mi_default_mem_resource;//std::pmr::get_default_resource();
        // g_runtime_memory = &m_mi_default_mem_resource;//std::pmr::get_default_resource();
        // g_frame_memory = &m_mi_default_mem_resource;//new std::pmr::monotonic_buffer_resource();
        // g_stack_memory = &m_mi_default_mem_resource;//new std::pmr::monotonic_buffer_resource();

        // // initialize default pmr resource 
        // std::pmr::set_default_resource(g_default_memory);
    // }
}