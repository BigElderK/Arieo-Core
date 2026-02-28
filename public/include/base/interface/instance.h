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

    // Single Instance template, conditionally adds members via flags
    template<typename TInstance, InstanceFlags Flags = InstanceFlags::None>
    class Instance
    {
    protected:
        TInstance m_instance;
        InstanceFlags m_flags = Flags;
        uint32_t m_type_hash = 0;
        
        // Only present when EnableRefCount is set
        struct RefCountStorage { std::atomic<size_t> m_ref_count{1}; };
        struct Empty {};
        
        NO_UNIQUE_ADDRESS
        std::conditional_t<hasFlag(Flags, InstanceFlags::EnableRefCount),
                        RefCountStorage, Empty> m_ref;

        template<typename> friend class Interface;
        template<typename> friend class Interop;

    public:
        template<typename... Args>
        Instance(Args&&... args) : m_instance(std::forward<Args>(args)...)
        {
            m_type_hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
        }

        TInstance* operator->() { return &m_instance; }
        const TInstance* operator->() const { return &m_instance; }

        // Only callable when EnableRefCount is active
        void addRef() requires (hasFlag(Flags, InstanceFlags::EnableRefCount))
        {
            m_ref.m_ref_count.fetch_add(1, std::memory_order_relaxed);
        }

        bool release() requires (hasFlag(Flags, InstanceFlags::EnableRefCount))
        {
            return m_ref.m_ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1;
        }

        size_t refCount() const requires (hasFlag(Flags, InstanceFlags::EnableRefCount))
        {
            return m_ref.m_ref_count.load(std::memory_order_relaxed);
        }

        template<typename TInterface>
        Base::Interop<TInterface> queryInterface()
        {
            return Base::Interop<TInterface>(static_cast<TInterface*>(&m_instance));
        }
    };
}