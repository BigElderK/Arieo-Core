#pragma once

namespace Arieo::Base
{
    template<typename T> class FunctionDelegate;

    template<typename R, typename... Args>
    class FunctionDelegate<R(Args...)> 
    {
        using FuncPtr = R(*)(void*, void*, Args...);
        void* m_object = nullptr;
        void* m_method = nullptr;
        FuncPtr m_function = nullptr;

    public:
        template<typename C>
        void bind(C* object, R(C::*method)(Args...)) {
            m_object = object;
            m_method = *(void**)(&method);

            m_function = [](void* obj, void* method, Args... args) -> R {
                // Reconstruct method pointer
                auto method_ptr = *reinterpret_cast<R(C::**)(Args...)>(&method);
                C* cobj = static_cast<C*>(obj);
                return (cobj->*method_ptr)(std::forward<Args>(args)...);
            };
        }
    
        R operator()(Args... args) const {
            return m_function(m_object, m_method, std::forward<Args>(args)...);
        }
    
        explicit operator bool() const { return m_function != nullptr; }
    };
}

