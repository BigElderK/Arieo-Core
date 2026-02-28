#pragma once
#include "base/delegates/function_delegate.h"
#include "core/logger/logger.h"
#include <cstddef>
#include <type_traits>

namespace Arieo::Base
{
    // Forward declaration of Interop (defined in interop.h)
    template<typename T>
    class Interop;

    // Define flags as an enum with power-of-2 values
    enum class InstanceFlags : uint32_t {
        None           = 0,
        EnableRefCount = 1 << 0,
        EnableLogging  = 1 << 1,
        // future flags...
    };
    
    // Enable bitwise operators
    constexpr InstanceFlags operator|(InstanceFlags a, InstanceFlags b) {
        return static_cast<InstanceFlags>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    constexpr bool hasFlag(InstanceFlags flags, InstanceFlags test) {
        return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(test)) != 0;
    }

    class RefControlBlock
    {
    protected:
        std::atomic<uint32_t> m_shared_ref_count{1}; 
        std::atomic<uint32_t> m_weak_ref_count{1}; // weak count starts at 1 (the strong ref group holds one weak ref)
        void (*m_delete_callback)() = nullptr;
        void (*m_destroy_block_callback)(RefControlBlock*) = nullptr;
    public:
        RefControlBlock(void (*delete_callback)(), void (*destroy_block)(RefControlBlock*) = nullptr) 
            : m_delete_callback(delete_callback), m_destroy_block_callback(destroy_block) {}

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
                if (m_delete_callback) m_delete_callback();
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
    template<typename TInstance, InstanceFlags Flags = InstanceFlags::None>
    class Instance
    {
    protected:
        TInstance m_instance;
        InstanceFlags m_flags = Flags;
        uint32_t m_type_hash = 0;
        
        struct Empty {};
        
        NO_UNIQUE_ADDRESS
        std::conditional_t<hasFlag(Flags, InstanceFlags::EnableRefCount),
                        RefControlBlock, Empty> m_ref;

        template<typename> friend class Interface;
        template<typename> friend class Interop;

    public:
        template<typename... Args>
        Instance(Args&&... args) 
            : m_instance(std::forward<Args>(args)...)
        {
            m_type_hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
        }

        template<typename... Args>
        Instance(void (*delete_callback_fun)(), Args&&... args) requires (hasFlag(Flags, InstanceFlags::EnableRefCount))
            : m_instance(std::forward<Args>(args)...), 
              m_ref(delete_callback_fun)
        {
            m_type_hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
        }

        TInstance* operator->() { return &m_instance; }
        const TInstance* operator->() const { return &m_instance; }

        template<typename TInterface>
        Base::Interop<TInterface> queryInterface()
        {
            return Base::Interop<TInterface>(static_cast<TInterface*>(&m_instance));
        }
    };
}