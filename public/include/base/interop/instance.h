#pragma once
#include "base/delegates/function_delegate.h"
#include "core/logger/logger.h"
#include <cstddef>
#include <type_traits>

namespace Arieo::Base
{
    // Forward declaration of InteropOld (defined in InteropOld.h)
    template<typename T>
    class InteropOld;

    namespace Interop
    {
        template<typename T>
        class WeakRef;

        template<typename T>
        class RawRef;
    }

    class RefControlBlock
    {
    protected:
        std::atomic<uint32_t> m_shared_ref_count{1}; 
        std::atomic<uint32_t> m_weak_ref_count{1}; // weak count starts at 1 (the strong ref group holds one weak ref)
        void* m_context = nullptr;
        void (*m_delete_callback)(void*) = nullptr;
        void (*m_destroy_block_callback)(RefControlBlock*) = nullptr;
    public:
        RefControlBlock(void* context, void (*delete_callback)(void*), void (*destroy_block)(RefControlBlock*)) 
            : m_context(context), m_delete_callback(delete_callback), m_destroy_block_callback(destroy_block) {}

        // --- Strong refs ---
        uint32_t addRef() 
        { 
            return m_shared_ref_count.fetch_add(1, std::memory_order_relaxed) + 1; 
        }
        uint32_t release() 
        {
            uint32_t prev = m_shared_ref_count.fetch_sub(1, std::memory_order_acq_rel);
            if(prev == 1) 
            {
                if (m_delete_callback) m_delete_callback(m_context);
                releaseWeak(); // release the weak ref held by the strong ref group
            }
            return prev - 1; 
        }
        uint32_t useCount() const
        {
            return m_shared_ref_count.load(std::memory_order_relaxed);
        }

        // Try to promote a weak ref to strong. Returns true if successful.
        bool tryAddRef()
        {
            uint32_t count = m_shared_ref_count.load(std::memory_order_relaxed);
            while (count != 0)
            {
                if (m_shared_ref_count.compare_exchange_weak(count, count + 1, std::memory_order_relaxed))
                    return true;
            }
            return false;
        }

        // --- Weak refs ---
        void addWeakRef()
        {
            m_weak_ref_count.fetch_add(1, std::memory_order_relaxed);
        }
        void releaseWeak()
        {
            if (m_weak_ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                if (m_destroy_block_callback) m_destroy_block_callback(this);
            }
        }
        uint32_t weakCount() const
        {
            return m_weak_ref_count.load(std::memory_order_relaxed);
        }
    };
    static_assert(std::is_standard_layout_v<RefControlBlock>,
                "RefControlBlock must be standard layout for stable ABI");
    static_assert(ct::DLLBoundarySafeParam<RefControlBlock*>,
                "RefControlBlock* must be safe to pass across DLL boundary");

    // Single Instance template, conditionally adds members via flags
    template<typename TInstance>
    class Instance
    {
    protected:
        TInstance m_instance;
        uint32_t m_type_hash = 0;
        
        struct Empty {};

        template<typename> friend class Interface;
        template<typename> friend class InteropOld;

    public:
        template<typename... Args>
        Instance(Args&&... args) 
            : m_instance(std::forward<Args>(args)...)
        {
            m_type_hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
        }

        TInstance* operator->() { return &m_instance; }
        const TInstance* operator->() const { return &m_instance; }

        template<typename TInterface>
        Base::Interop::RawRef<TInterface> queryInterface()
        {
            return Base::Interop::RawRef<TInterface>(static_cast<TInterface*>(&m_instance));
        }

        uint32_t getTypeHash() const { return m_type_hash; }
    };
}



