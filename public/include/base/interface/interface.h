#pragma once
#include "base/delegates/function_delegate.h"
#include "core/logger/logger.h"
#include <cstddef>
#include <type_traits>


#define METADATA(x) [[clang::annotate(#x)]]

namespace Arieo::Core
{
    class ModuleManager;
}

namespace Arieo::Base
{
    // Value types crossing DLL boundaries must be standard layout, trivially copyable, and have no padding
    template<typename T>
    inline constexpr bool DLLBoundarySafeCheck =
        std::is_standard_layout_v<T> &&
        std::is_trivially_copyable_v<T> &&
        std::has_unique_object_representations_v<T>;

    // A parameter/return type is safe across DLL if it's:
    //   - void (return only)
    //   - a pointer or reference (caller/callee share address space)
    //   - a value type that passes DLLBoundarySafeCheck
    template<typename T>
    inline constexpr bool DLLBoundarySafeParam =
        std::is_void_v<T> ||
        std::is_pointer_v<T> ||
        std::is_reference_v<T> ||
        DLLBoundarySafeCheck<std::remove_cv_t<T>>;

    // Decompose a member function pointer and check all parameter + return types
    template<typename T>
    struct DLLSafeMemberFunctionCheck : std::false_type {};

    template<typename R, typename C, typename... Args>
    struct DLLSafeMemberFunctionCheck<R(C::*)(Args...)>
        : std::bool_constant<DLLBoundarySafeParam<R> && (DLLBoundarySafeParam<Args> && ...)> {};

    // const-qualified overload
    template<typename R, typename C, typename... Args>
    struct DLLSafeMemberFunctionCheck<R(C::*)(Args...) const>
        : std::bool_constant<DLLBoundarySafeParam<R> && (DLLBoundarySafeParam<Args> && ...)> {};
}

namespace Arieo::Base
{
    // template <typename TInstance, typename TInterface>
    // inline TInstance* castInterfaceToInstance(TInterface* interface)
    // {
    //     return static_cast<TInstance*>(interface);
    // }
    template<typename TInterface>
    class Interface;

    template<typename TInstance>
    class Instance;

    template<typename TInstance>
    class Instance
    {
    protected:
        TInstance m_instance;
        size_t m_type_hash;

        template<typename>
        friend class Interface;
    public:
        template<typename... Args>
        Instance(Args&&... args) : m_instance(std::forward<Args>(args)...) 
        {
            const size_t type_hash = std::hash<std::string_view>{}(typeid(TInstance).name());
            m_type_hash = type_hash;
        }

        TInstance* operator->() { return &m_instance; }
        const TInstance* operator->() const { return &m_instance; }

        template<typename TInterface>
        Base::Interface<TInterface> queryInterface() { return Base::Interface<TInterface>(this->operator->()); }
        template<typename TInterface>
        Base::Interface<TInterface> queryInterface() const { return Base::Interface<TInterface>(this->operator->()); }
    };

    template<typename TInterface>
    class Interface    
    {
    private:
    
    private:
        TInterface* m_interface = nullptr;
        Interface(TInterface* interface) : m_interface(interface) {}
        friend class Arieo::Core::ModuleManager;

        template<typename T>
        friend class Instance;
    public:
        Interface() = default;
        Interface(std::nullptr_t) : m_interface(nullptr) {}
        Interface(const Interface&) noexcept = default;
        Interface(Interface&&) noexcept = default;
        Interface& operator=(const Interface&) noexcept = default;
        Interface& operator=(Interface&&) noexcept = default;
        ~Interface() = default;

        Interface& operator=(std::nullptr_t)
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

        bool operator==(Interface<TInterface> const& other) const { return m_interface == other.m_interface; }
        bool operator!=(Interface<TInterface> const& other) const { return m_interface != other.m_interface; }

        template<typename TInstance, typename... Args>
        static Interface<TInterface> createAs(Args&&... args)
        {
            const std::size_t hash = std::hash<std::string_view>{}(typeid(TInstance).name());
            Core::Logger::trace("Creating instance of type {} with hash {}", typeid(TInstance).name(), hash);
            TInstance* new_instance = new (Arieo::Base::Memory::malloc(sizeof(TInstance) + sizeof(std::size_t))) TInstance(std::forward<Args>(args)...);
            std::size_t* type_hash_ptr = reinterpret_cast<std::size_t*>(reinterpret_cast<std::byte*>(new_instance) + sizeof(TInstance));
            *type_hash_ptr = hash;

            return Interface<TInterface>(new_instance);
        }

        template <typename TInstance>
        TInstance* castTo()
        {
            const std::size_t hash = std::hash<std::string_view>{}(typeid(TInstance).name());
            std::size_t* type_hash_ptr = reinterpret_cast<std::size_t*>(reinterpret_cast<std::byte*>(static_cast<TInstance*>(m_interface)) + sizeof(TInstance));
            if(*type_hash_ptr != hash)            
            {
                Core::Logger::fatal("Type hash mismatch when casting instance. Expected: {}, Actual: {}", hash, *type_hash_ptr);
                return nullptr;
            }
            return static_cast<TInstance*>(m_interface);
        }

        template<typename TInstance>
        void destroyAs()
        {
            const std::size_t hash = std::hash<std::string_view>{}(typeid(TInstance).name());
            std::size_t* type_hash_ptr = reinterpret_cast<std::size_t*>(reinterpret_cast<std::byte*>(m_interface) + sizeof(TInstance));
            
            if(*type_hash_ptr != hash)
            {
                Core::Logger::fatal("Type hash mismatch when destroying instance. Expected: {}, Actual: {}", hash, *type_hash_ptr);
                return;
            }

            reinterpret_cast<TInstance*>(m_interface)->TInstance::~TInstance();
            Arieo::Base::Memory::free(m_interface);
            m_interface = nullptr;
        }
    };

    static_assert(Base::DLLBoundarySafeCheck<Interface<void>>, "Interface<T> must be DLL boundary safe");

    template<typename T>
    class InterfaceInfo
    {
    };

    namespace Parameter
    {
        class String
        {
        private:
            void* buf = nullptr;
            size_t size = 0;
        public:
            String() = delete;
            String(const String&) = delete;
            String(String&&) = delete;

            String(const std::string& str)
            {
                size = str.size();
                buf = (void*)str.c_str();
            }

            String(const char str[])
            {
                size = std::strlen(str);
                buf = (void*)str;
            }

            std::string getString() const
            {
                return std::string((char*)buf, size);
            }
        };
        static_assert(Base::DLLBoundarySafeCheck<String>, "Parameter::String must be DLL boundary safe");
    };
}

// Hash and equality for Interface so it can be used in unordered_set/map
namespace std {
    template <typename TInterface>
    struct hash<Arieo::Base::Interface<TInterface>> {
        std::size_t operator()(const Arieo::Base::Interface<TInterface>& iface) const noexcept {
            return reinterpret_cast<std::size_t>(iface.operator->());
        }
    };
    template <typename TInterface>
    struct equal_to<Arieo::Base::Interface<TInterface>> {
        bool operator()(const Arieo::Base::Interface<TInterface>& lhs, const Arieo::Base::Interface<TInterface>& rhs) const noexcept {
            return lhs.operator->() == rhs.operator->();
        }
    };
}