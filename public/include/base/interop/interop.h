#pragma once
#include <cstddef>
#include <type_traits>
#include "base/compile/ct_check.h"
#include "base/interop/instance.h"
#include "core/logger/logger.h"

namespace Arieo::Core
{
    class ModuleManager;
}

namespace Arieo::Base::Interop
{
    template<typename T>
    class SharedRef
    {
    protected:
        T* m_ptr = nullptr;
        RefControlBlock* m_ref_control_block = nullptr;

    public:
        // --- Constructors ---
        SharedRef() noexcept = default;
        SharedRef(std::nullptr_t) noexcept {}

        SharedRef(T* ptr, RefControlBlock* ref_control_block) noexcept
            : m_ptr(ptr), m_ref_control_block(ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addRef();
        }

        // Copy
        SharedRef(const SharedRef& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addRef();
        }

        // Move
        SharedRef(SharedRef&& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            other.m_ptr = nullptr;
            other.m_ref_control_block = nullptr;
        }

        // Aliasing constructor (like shared_ptr aliasing ctor)
        template<typename U>
        SharedRef(const SharedRef<U>& other, T* ptr) noexcept
            : m_ptr(ptr), m_ref_control_block(other.m_ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addRef();
        }

        // Converting constructor (implicit upcast)
        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedRef(const SharedRef<U>& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addRef();
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedRef(SharedRef<U>&& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            other.m_ptr = nullptr;
            other.m_ref_control_block = nullptr;
        }

        // --- Destructor ---
        ~SharedRef()
        {
            if (m_ref_control_block) m_ref_control_block->release();
        }

        // --- Assignment ---
        SharedRef& operator=(const SharedRef& other) noexcept
        {
            if (this != &other)
            {
                SharedRef tmp(other);
                swap(tmp);
            }
            return *this;
        }

        SharedRef& operator=(SharedRef&& other) noexcept
        {
            if (this != &other)
            {
                SharedRef tmp(std::move(other));
                swap(tmp);
            }
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedRef& operator=(const SharedRef<U>& other) noexcept
        {
            SharedRef tmp(other);
            swap(tmp);
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        SharedRef& operator=(SharedRef<U>&& other) noexcept
        {
            SharedRef tmp(std::move(other));
            swap(tmp);
            return *this;
        }

        SharedRef& operator=(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        // --- Modifiers ---
        void reset() noexcept
        {
            SharedRef tmp;
            swap(tmp);
        }

        void swap(SharedRef& other) noexcept
        {
            std::swap(m_ptr, other.m_ptr);
            std::swap(m_ref_control_block, other.m_ref_control_block);
        }

        // --- Observers ---
        T* get() const noexcept { return m_ptr; }

        template<typename U = T>
        U& operator*() const noexcept requires (!std::is_void_v<U>) { return *m_ptr; }
        T* operator->() const noexcept { return m_ptr; }

        uint32_t useCount() const noexcept
        {
            return m_ref_control_block ? m_ref_control_block->useCount() : 0;
        }

        explicit operator bool() const noexcept { return m_ptr != nullptr; }

        // --- Comparisons ---
        bool operator==(const SharedRef& other) const noexcept { return m_ptr == other.m_ptr; }
        bool operator!=(const SharedRef& other) const noexcept { return m_ptr != other.m_ptr; }
        bool operator<(const SharedRef& other) const noexcept { return m_ptr < other.m_ptr; }
        bool operator==(std::nullptr_t) const noexcept { return m_ptr == nullptr; }
        bool operator!=(std::nullptr_t) const noexcept { return m_ptr != nullptr; }

        // Allow SharedRef<U> to access our internals for converting ctors
        template<typename U>
        friend class SharedRef;

        template<typename U>
        friend class WeakRef;

        template <typename TInstance>
        TInstance* castToInstance()
        {
            const uint32_t hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
            uint32_t* type_hash_ptr = reinterpret_cast<uint32_t*>(reinterpret_cast<std::byte*>(static_cast<TInstance*>(m_ptr)) + sizeof(TInstance));
            if(*type_hash_ptr != hash)            
            {
                Core::Logger::fatal("Type hash mismatch when casting instance. Expected: {}, Actual: {}", hash, *type_hash_ptr);
                return nullptr;
            }
            return static_cast<TInstance*>(m_ptr);
        }
    };
    static_assert(Base::ct::DLLBoundaryLayoutSafe<SharedRef<void>>, "SharedRef<T> must be DLL boundary safe");

    template<typename T>
    class WeakRef
    {
    protected:
        T* m_ptr = nullptr;
        RefControlBlock* m_ref_control_block = nullptr;
    public:
        // --- Constructors ---
        WeakRef() noexcept = default;

        // From SharedRef
        WeakRef(const SharedRef<T>& shared) noexcept
            : m_ptr(shared.m_ptr), m_ref_control_block(shared.m_ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addWeakRef();
        }

        // Converting from SharedRef<U>
        template<typename U>
            requires std::is_convertible_v<U*, T*>
        WeakRef(const SharedRef<U>& shared) noexcept
            : m_ptr(shared.m_ptr), m_ref_control_block(shared.m_ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addWeakRef();
        }

        // Copy
        WeakRef(const WeakRef& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addWeakRef();
        }

        // Move
        WeakRef(WeakRef&& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            other.m_ptr = nullptr;
            other.m_ref_control_block = nullptr;
        }

        // Converting copy
        template<typename U>
            requires std::is_convertible_v<U*, T*>
        WeakRef(const WeakRef<U>& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            if (m_ref_control_block) m_ref_control_block->addWeakRef();
        }

        // Converting move
        template<typename U>
            requires std::is_convertible_v<U*, T*>
        WeakRef(WeakRef<U>&& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            other.m_ptr = nullptr;
            other.m_ref_control_block = nullptr;
        }

        // --- Destructor ---
        ~WeakRef()
        {
            if (m_ref_control_block) m_ref_control_block->releaseWeak();
        }

        // --- Assignment ---
        WeakRef& operator=(const WeakRef& other) noexcept
        {
            if (this != &other)
            {
                WeakRef tmp(other);
                swap(tmp);
            }
            return *this;
        }

        WeakRef& operator=(WeakRef&& other) noexcept
        {
            if (this != &other)
            {
                WeakRef tmp(std::move(other));
                swap(tmp);
            }
            return *this;
        }

        WeakRef& operator=(const SharedRef<T>& shared) noexcept
        {
            WeakRef tmp(shared);
            swap(tmp);
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        WeakRef& operator=(const SharedRef<U>& shared) noexcept
        {
            WeakRef tmp(shared);
            swap(tmp);
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        WeakRef& operator=(const WeakRef<U>& other) noexcept
        {
            WeakRef tmp(other);
            swap(tmp);
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        WeakRef& operator=(WeakRef<U>&& other) noexcept
        {
            WeakRef tmp(std::move(other));
            swap(tmp);
            return *this;
        }

        // --- Modifiers ---
        void reset() noexcept
        {
            WeakRef tmp;
            swap(tmp);
        }

        void swap(WeakRef& other) noexcept
        {
            std::swap(m_ptr, other.m_ptr);
            std::swap(m_ref_control_block, other.m_ref_control_block);
        }

        // --- Observers ---
        T* get() const noexcept { return m_ptr; }

        uint32_t useCount() const noexcept
        {
            return m_ref_control_block ? m_ref_control_block->useCount() : 0;
        }

        bool expired() const noexcept
        {
            return useCount() == 0;
        }

        // Attempt to obtain a SharedRef. Returns empty SharedRef if expired.
        SharedRef<T> lock() const noexcept
        {
            if (m_ref_control_block && m_ref_control_block->tryAddRef())
            {
                // tryAddRef already incremented the strong count,
                // construct SharedRef without addRef
                SharedRef<T> result;
                result.m_ptr = m_ptr;
                result.m_ref_control_block = m_ref_control_block;
                return result;
            }
            return SharedRef<T>();
        }

        template<typename U>
        friend class WeakRef;

        template<typename U>
        friend class SharedRef;
    };
    static_assert(Base::ct::DLLBoundaryLayoutSafe<WeakRef<void>>, "WeakRef<T> must be DLL boundary safe");

    template<typename T>
    class UniqueRef
    {
    protected:
        T* m_ptr = nullptr;
        RefControlBlock* m_ref_control_block = nullptr;

    public:
        // --- Constructors ---
        UniqueRef() noexcept = default;
        UniqueRef(std::nullptr_t) noexcept {}

        UniqueRef(T* ptr, RefControlBlock* ref_control_block) noexcept
            : m_ptr(ptr), m_ref_control_block(ref_control_block) {}

        // No copy
        UniqueRef(const UniqueRef&) = delete;
        UniqueRef& operator=(const UniqueRef&) = delete;

        // Move
        UniqueRef(UniqueRef&& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            other.m_ptr = nullptr;
            other.m_ref_control_block = nullptr;
        }

        // Converting move
        template<typename U>
            requires std::is_convertible_v<U*, T*>
        UniqueRef(UniqueRef<U>&& other) noexcept
            : m_ptr(other.m_ptr), m_ref_control_block(other.m_ref_control_block)
        {
            other.m_ptr = nullptr;
            other.m_ref_control_block = nullptr;
        }

        // --- Destructor ---
        ~UniqueRef()
        {
            if (m_ref_control_block) m_ref_control_block->release();
        }

        // --- Assignment ---
        UniqueRef& operator=(UniqueRef&& other) noexcept
        {
            if (this != &other)
            {
                UniqueRef tmp(std::move(other));
                swap(tmp);
            }
            return *this;
        }

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        UniqueRef& operator=(UniqueRef<U>&& other) noexcept
        {
            UniqueRef tmp(std::move(other));
            swap(tmp);
            return *this;
        }

        UniqueRef& operator=(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        // --- Modifiers ---
        void reset() noexcept
        {
            UniqueRef tmp;
            swap(tmp);
        }

        T* release() noexcept
        {
            T* ptr = m_ptr;
            m_ptr = nullptr;
            m_ref_control_block = nullptr;
            return ptr;
        }

        void swap(UniqueRef& other) noexcept
        {
            std::swap(m_ptr, other.m_ptr);
            std::swap(m_ref_control_block, other.m_ref_control_block);
        }

        // --- Observers ---
        T* get() const noexcept { return m_ptr; }

        template<typename U = T>
        U& operator*() const noexcept requires (!std::is_void_v<U>) { return *m_ptr; }
        T* operator->() const noexcept { return m_ptr; }

        explicit operator bool() const noexcept { return m_ptr != nullptr; }

        // --- Comparisons ---
        bool operator==(const UniqueRef& other) const noexcept { return m_ptr == other.m_ptr; }
        bool operator!=(const UniqueRef& other) const noexcept { return m_ptr != other.m_ptr; }
        bool operator==(std::nullptr_t) const noexcept { return m_ptr == nullptr; }
        bool operator!=(std::nullptr_t) const noexcept { return m_ptr != nullptr; }

        template<typename U>
        friend class UniqueRef;
    };
    static_assert(Base::ct::DLLBoundaryLayoutSafe<UniqueRef<void>>, "UniqueRef<T> must be DLL boundary safe");

    template<typename T>
    class RawRef
    {
    protected:
        T* m_ptr = nullptr;

    public:
        RawRef() noexcept = default;
        RawRef(std::nullptr_t) noexcept {}
        RawRef(T* ptr) noexcept : m_ptr(ptr) {}

        RawRef(const RawRef&) noexcept = default;
        RawRef(RawRef&&) noexcept = default;
        RawRef& operator=(const RawRef&) noexcept = default;
        RawRef& operator=(RawRef&&) noexcept = default;
        ~RawRef() = default;

        template<typename U>
            requires std::is_convertible_v<U*, T*>
        RawRef(const RawRef<U>& other) noexcept : m_ptr(other.get()) {}

        RawRef& operator=(std::nullptr_t) noexcept { m_ptr = nullptr; return *this; }
        RawRef& operator=(T* ptr) noexcept { m_ptr = ptr; return *this; }

        void reset() noexcept { m_ptr = nullptr; }

        T* get() const noexcept { return m_ptr; }
        template<typename U = T>
        U& operator*() const noexcept requires (!std::is_void_v<U>) { return *m_ptr; }
        T* operator->() const noexcept { return m_ptr; }

        explicit operator bool() const noexcept { return m_ptr != nullptr; }

        bool operator==(const RawRef& other) const noexcept { return m_ptr == other.m_ptr; }
        bool operator!=(const RawRef& other) const noexcept { return m_ptr != other.m_ptr; }
        bool operator<(const RawRef& other) const noexcept { return m_ptr < other.m_ptr; }
        bool operator==(std::nullptr_t) const noexcept { return m_ptr == nullptr; }
        bool operator!=(std::nullptr_t) const noexcept { return m_ptr != nullptr; }

        template<typename U>
        friend class RawRef;

        template <typename TInstance>
        TInstance* castToInstance()
        {
            const uint32_t hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
            uint32_t* type_hash_ptr = reinterpret_cast<uint32_t*>(reinterpret_cast<std::byte*>(static_cast<TInstance*>(m_ptr)) + sizeof(TInstance));
            if(*type_hash_ptr != hash)            
            {
                Core::Logger::fatal("Type hash mismatch when casting instance. Expected: {}, Actual: {}", hash, *type_hash_ptr);
                return nullptr;
            }
            return static_cast<TInstance*>(m_ptr);
        }

        template<typename TInstance, typename... Args>
        static RawRef<T> createAs(Args&&... args)
        {
            Instance<TInstance>* new_instance = new (Base::Memory::malloc(sizeof(Instance<TInstance>))) Instance<TInstance>(std::forward<Args>(args)...);
            return RawRef<T>(static_cast<T*>(new_instance->operator->()));
        }

        template<typename TInstance>
        static void destroyAs(RawRef<T>&& interface)
        {
            const uint32_t hash = static_cast<uint32_t>(std::hash<std::string_view>{}(typeid(TInstance).name()));
            Instance<TInstance>* instance = reinterpret_cast<Instance<TInstance>*>(interface.m_ptr);
            
            if(instance->getTypeHash() != hash)
            {
                Core::Logger::fatal("Type hash mismatch when destroying instance. Expected: {}, Actual: {}", hash, instance->getTypeHash());
                return;
            }

            instance->~Instance<TInstance>();
            Arieo::Base::Memory::free(instance);

            interface.m_ptr = nullptr;
        }
    };
    static_assert(Base::ct::DLLBoundaryLayoutSafe<RawRef<void>>, "RawRef<T> must be DLL boundary safe");

    template<typename TInterface, typename TInstance, typename... Args>
    static SharedRef<TInterface> createInstance(Args&&... args)
    {
        // Allocate memory for T + type hash
        Instance<TInstance>* new_instance = new (Base::Memory::malloc(sizeof(Instance<TInstance>))) Instance<TInstance>(std::forward<Args>(args)...);
        RefControlBlock* ref_block = new (Base::Memory::malloc(sizeof(RefControlBlock))) RefControlBlock(
            new_instance,
            [](void* context) -> void{
                auto* instance = static_cast<Instance<TInstance>*>(context);
                instance->~Instance<TInstance>();
                Base::Memory::free(instance);
            },
            [](RefControlBlock* this_block) -> void{
                this_block->~RefControlBlock();
                Base::Memory::free(this_block);
            }
        );

        return SharedRef<TInterface>(static_cast<TInterface*>(new_instance->operator->()), ref_block);
    }
}

// Hash and equality for Interface so it can be used in unordered_set/map
namespace std {
    template <typename TInterface>
    struct hash<Arieo::Base::Interop::RawRef<TInterface>> {
        std::size_t operator()(const Arieo::Base::Interop::RawRef<TInterface>& iface) const noexcept {
            return reinterpret_cast<std::size_t>(iface.operator->());
        }
    };
    template <typename TInterface>
    struct equal_to<Arieo::Base::Interop::RawRef<TInterface>> {
        bool operator()(const Arieo::Base::Interop::RawRef<TInterface>& lhs, const Arieo::Base::Interop::RawRef<TInterface>& rhs) const noexcept {
            return lhs.operator->() == rhs.operator->();
        }
    };

    // Hash and equality for SharedRef so it can be used in unordered_set/map
    template <typename T>
    struct hash<Arieo::Base::Interop::SharedRef<T>> {
        std::size_t operator()(const Arieo::Base::Interop::SharedRef<T>& ref) const noexcept {
            return reinterpret_cast<std::size_t>(ref.get());
        }
    };
    template <typename T>
    struct equal_to<Arieo::Base::Interop::SharedRef<T>> {
        bool operator()(const Arieo::Base::Interop::SharedRef<T>& lhs, const Arieo::Base::Interop::SharedRef<T>& rhs) const noexcept {
            return lhs.get() == rhs.get();
        }
    };

    // Hash and equality for WeakRef so it can be used in unordered_set/map
    template <typename T>
    struct hash<Arieo::Base::Interop::WeakRef<T>> {
        std::size_t operator()(const Arieo::Base::Interop::WeakRef<T>& ref) const noexcept {
            return reinterpret_cast<std::size_t>(ref.get());
        }
    };
    template <typename T>
    struct equal_to<Arieo::Base::Interop::WeakRef<T>> {
        bool operator()(const Arieo::Base::Interop::WeakRef<T>& lhs, const Arieo::Base::Interop::WeakRef<T>& rhs) const noexcept {
            return lhs.get() == rhs.get();
        }
    };
}

namespace Arieo::Base::Interop
{
    class StringView
    {
    private:
        void* buf = nullptr;
        size_t size = 0;
    public:
        StringView() = delete;
        StringView(const StringView&) noexcept = delete;
        StringView(StringView&&) noexcept = delete;
        StringView& operator=(const StringView&) noexcept = delete;
        StringView& operator=(StringView&&) noexcept = delete;
        ~StringView() = default;

        StringView(const std::string_view& str)
            : buf((void*)str.data()), size(str.size()) {}

        StringView(const char str[])
            : buf((void*)str), size(std::strlen(str)) {}

        std::string getString() const
        {
            return std::string((char*)buf, size);
        }
    };
    static_assert(Base::ct::DLLBoundarySafeCheck<StringView>, "StringView must be DLL boundary safe");
}

