#pragma once
#include <cstddef>
#include <type_traits>
#include "base/compile/ct_check.h"
#include "base/interface/instance.h"
#include "core/logger/logger.h"

namespace Arieo::Core
{
    class ModuleManager;
}


namespace Arieo::Base
{
    // Forward declaration (defined in instance.h)
    enum class InstanceFlags : unsigned;

    class IReferenceCounted
    {
    public:        
        virtual void addRef() = 0;
        virtual void release() = 0;
    };
    class IBufferView
    {
    public:
        virtual void* getBuffer() const = 0;
        virtual size_t getSize() const = 0;
    };

    class IBuffer
        : IReferenceCounted
    {
    public:
        virtual void* getBuffer() const = 0;
        virtual size_t getSize() const = 0;
    };
}

namespace Arieo::Base
{
    template<typename T>
    class Interop;

    template<>
    class Interop<std::string_view>
    {
    private:
        void* buf = nullptr;
        size_t size = 0;
    public:
        Interop() = delete;
        Interop(const Interop&) noexcept = delete;
        Interop(Interop&&) noexcept = delete;
        Interop& operator=(const Interop&) noexcept = delete;
        Interop& operator=(Interop&&) noexcept = delete;
        ~Interop() = default;

        Interop(const std::string_view& str)
            : buf((void*)str.data()), size(str.size()) {}

        Interop(const char str[])
            : buf((void*)str), size(std::strlen(str)) {}

        std::string getString() const
        {
            return std::string((char*)buf, size);
        }
    };
    static_assert(Base::ct::DLLBoundarySafeCheck<Interop<std::string_view>>, "Interop<std::string_view> must be DLL boundary safe");

    template<>
    class Interop<IBufferView>
    {
    private:
        void* m_buf = nullptr;
        size_t m_size = 0;
    public:
        Interop() = delete;
        Interop(const Interop&) noexcept = delete;
        Interop(Interop&&) noexcept = delete;
        Interop& operator=(const Interop&) noexcept = delete;
        Interop& operator=(Interop&&) noexcept = delete;
        ~Interop() = default;

        Interop(void* data, size_t len) : m_buf(data), m_size(len) {}

        void* getBuffer() const { return m_buf; }
        size_t getSize() const { return m_size; }
    };
    static_assert(Base::ct::DLLBoundarySafeCheck<Interop<IBufferView>>, "Interop<IBufferView> must be DLL boundary safe");

    template<>
    class Interop<IBuffer>
    {
    private:
        void* m_buf = nullptr;
        size_t m_size = 0;
        void (*m_release_func)(void*, size_t) = nullptr;

    public:
        Interop() = delete;
        Interop(const Interop&) noexcept = default;
        Interop(Interop&&) noexcept = default;
        Interop& operator=(const Interop&) noexcept = default;
        Interop& operator=(Interop&&) noexcept = default;
        ~Interop() = default;

        Interop(void* data, size_t len, void (*release_func)(void*, size_t)) 
            : m_buf(data), m_size(len), m_release_func(release_func) {}

        void* getBuffer() const { return m_buf; }
        size_t getSize() const { return m_size; }

        void release()
        {
            if(m_release_func)
            {
                m_release_func(m_buf, m_size);
                m_buf = nullptr;
                m_size = 0;
            }
        }
    };
    static_assert(Base::ct::DLLBoundarySafeCheck<Interop<IBufferView>>, "Interop<IBufferView> must be DLL boundary safe");

    template<class TInterface>
    class Interop    
    {
    private:
    
    private:
        TInterface* m_interface = nullptr;
        Interop(TInterface* interface) : m_interface(interface) {}
        friend class Arieo::Core::ModuleManager;

        template<typename T, InstanceFlags>
        friend class Instance;
    public:
        Interop() = default;
        Interop(std::nullptr_t) : m_interface(nullptr) {}
        Interop(const Interop&) noexcept = default;
        Interop(Interop&&) noexcept = default;
        Interop& operator=(const Interop&) noexcept = default;
        Interop& operator=(Interop&&) noexcept = default;
        ~Interop() = default;

        Interop& operator=(std::nullptr_t)
        {
            m_interface = nullptr;
            return *this;
        }

        TInterface* operator->() { return m_interface; }
        const TInterface* operator->() const { return m_interface; }

        // TInterface& operator*() { return *m_interface; }
        // const TInterface& operator*() const { return *m_interface; }

        // operator TInterface*() { return m_interface; }
        // operator const TInterface*() const { return m_interface; }

        explicit operator bool() const { return m_interface != nullptr; }

        bool operator==(std::nullptr_t) const { return m_interface == nullptr; }
        bool operator!=(std::nullptr_t) const { return m_interface != nullptr; }

        bool operator==(Interop<TInterface> const& other) const { return m_interface == other.m_interface; }
        bool operator!=(Interop<TInterface> const& other) const { return m_interface != other.m_interface; }

        template<typename TInstance, InstanceFlags Flags = InstanceFlags::None, typename... Args>
        static Interop<TInterface> createAs(Args&&... args)
        {
            Instance<TInstance>* new_instance = new (Base::Memory::malloc(sizeof(Instance<TInstance>))) Instance<TInstance, Flags>(std::forward<Args>(args)...);
            return new_instance->template queryInterface<TInterface>();
        }

        template<typename TInstance>
        static void destroyAs(Interop<TInterface>&& interop)
        {
            const uint32_t hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
            Instance<TInstance>* instance = reinterpret_cast<Instance<TInstance>*>(interop.m_interface);
            
            if(instance->m_type_hash != hash)
            {
                Core::Logger::fatal("Type hash mismatch when destroying instance. Expected: {}, Actual: {}", hash, instance->m_type_hash);
                return;
            }

            instance->~Instance<TInstance>();
            Arieo::Base::Memory::free(interop.m_interface);

            interop.m_interface = nullptr;
        }

        template <typename TInstance>
        TInstance* castTo()
        {
            const uint32_t hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
            uint32_t* type_hash_ptr = reinterpret_cast<uint32_t*>(reinterpret_cast<std::byte*>(static_cast<TInstance*>(m_interface)) + sizeof(TInstance) + sizeof(uint32_t));
            if(*type_hash_ptr != hash)            
            {
                Core::Logger::fatal("Type hash mismatch when casting instance. Expected: {}, Actual: {}", hash, *type_hash_ptr);
                return nullptr;
            }
            return static_cast<TInstance*>(m_interface);
        }
    };
    static_assert(Base::ct::DLLBoundarySafeCheck<Interop<void>>, "Interop<T> must be DLL boundary safe");
}

// Hash and equality for Interface so it can be used in unordered_set/map
namespace std {
    template <typename TInterface>
    struct hash<Arieo::Base::Interop<TInterface>> {
        std::size_t operator()(const Arieo::Base::Interop<TInterface>& iface) const noexcept {
            return reinterpret_cast<std::size_t>(iface.operator->());
        }
    };
    template <typename TInterface>
    struct equal_to<Arieo::Base::Interop<TInterface>> {
        bool operator()(const Arieo::Base::Interop<TInterface>& lhs, const Arieo::Base::Interop<TInterface>& rhs) const noexcept {
            return lhs.operator->() == rhs.operator->();
        }
    };
}