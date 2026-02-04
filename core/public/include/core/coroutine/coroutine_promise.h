#pragma once
#include <coroutine>
#include <type_traits>
#include "core/coroutine/task.h"
namespace Arieo::Core::Coroutine
{
    class Task;

    template<typename T>
    class CorHandle;

    template<typename T>
    class Promise;

    class YieldUntil final
    {
    private:
        friend class PromiseBase;
        std::function<bool()> m_update_fun;
    public:
        YieldUntil(std::function<bool()>&& resume_check_fun) noexcept
            : m_update_fun(std::move(resume_check_fun))
        {

        }
    };

    class YieldUpdate final
    {
    private:
        friend class PromiseBase;
        std::function<bool(Task&)> m_update_fun;
    public:
        YieldUpdate(std::function<bool(Task&)>&& update_func) noexcept
            : m_update_fun(std::move(update_func))
        {

        }
    };

    template<class T>
    class YieldUpdateOnce final
    {
    private:
        friend class PromiseBase;
        std::function<T(Task&)> m_update_fun;
    public:
        YieldUpdateOnce(std::function<T(Task&)>&& update_func) noexcept
            : m_update_fun(std::move(update_func))
        {

        }
    };

    template<class T>
    class YieldSubCoroutine final
    {
    private:
        friend class PromiseBase;
        CorHandle<T> m_sub_coroutine;
    public:
        YieldSubCoroutine(CorHandle<T>&& sub_coroutine) noexcept
            : m_sub_coroutine(std::move(sub_coroutine))
        {
        }

        YieldSubCoroutine(const YieldSubCoroutine& other) = delete;
        YieldSubCoroutine& operator=(const YieldSubCoroutine& other) = delete;
    };

    template<class T>
    class CreateParallelCoroutine final
    {
    private:
        friend class PromiseBase;
        CorHandle<T> m_parallel_coroutine;
    public:
        CreateParallelCoroutine(CorHandle<T>&& sub_coroutine) noexcept
            : m_parallel_coroutine(std::move(sub_coroutine))
        {
        }

        CreateParallelCoroutine(const CreateParallelCoroutine& other) = delete;
        CreateParallelCoroutine& operator=(const CreateParallelCoroutine& other) = delete;
    };

    class PromiseBase
    {
    private:
        friend class Task;
        std::function<bool(Task&)> m_update_fun = nullptr;
    
    public:
        enum class OpFlag : uint32_t
        {
            NONE = 0,
            UPDATE_IMMEDIATELY = 1             
        };
        OpFlag m_op_flag = OpFlag::NONE;

        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}

        auto yield_value(std::suspend_always&& yield_value) noexcept
        {
            return std::suspend_always{};
        }

        auto yield_value(YieldUntil&& yield_until)
        {
            m_update_fun = [yield_until = std::move(yield_until)](Task&)
            {
                return yield_until.m_update_fun();
            };
            return std::suspend_always{};
        }

        auto yield_value(YieldUpdate&& yield_update)
        {
            m_update_fun = [yield_update = std::move(yield_update)](Task& running_task)
            {
                return yield_update.m_update_fun(running_task);
            };
            m_op_flag = (OpFlag)((std::uint32_t)m_op_flag | (std::uint32_t)OpFlag::UPDATE_IMMEDIATELY);
            return std::suspend_always{};
        }

        template<class T>
        auto yield_value(YieldUpdateOnce<T>&& yield_update_once)
        {
            if constexpr (requires { requires !std::is_void_v<T>; }) 
            {
                T* ret_t = Base::newT<T>();
                class Awaiter : public std::suspend_always
                {
                public:
                    Awaiter(const Awaiter&) = delete;
                    Awaiter& operator=(const Awaiter& other) = delete;

                    Awaiter()
                    {

                    }

                    Awaiter(Awaiter&& other)
                        : m_ret(other.m_ret)
                    {
                        other.m_ret = nullptr;
                    }

                    T* m_ret = nullptr;
                    T await_resume() const
                    {
                        return *m_ret;
                    }
                    ~Awaiter()
                    {
                        if(m_ret != nullptr)
                        {
                            Base::deleteT(m_ret);
                        }
                    }
                };
                Awaiter ret;
                ret.m_ret = ret_t;

                m_update_fun = [yield_update_once = std::move(yield_update_once), ret_t](Task& running_task) mutable
                {
                    *ret_t = yield_update_once.m_update_fun(running_task);
                    return true;
                };

                m_op_flag = (OpFlag)((std::uint32_t)m_op_flag | (std::uint32_t)OpFlag::UPDATE_IMMEDIATELY);
                return ret;
            }
            else
            {
                m_update_fun = [yield_update_once = std::move(yield_update_once)](Task& running_task)
                {
                    yield_update_once.m_update_fun(running_task);
                    return true;
                };

                m_op_flag = (OpFlag)((std::uint32_t)m_op_flag | (std::uint32_t)OpFlag::UPDATE_IMMEDIATELY);
                return std::suspend_always{};
            }
        }

        template<typename T>
        auto yield_value(YieldSubCoroutine<T>&& yield_for_sub_coroutine)
        {
            class Awaiter : public std::suspend_always
            {
            public:
                typename Core::Coroutine::CorHandle<T>::StdHandle m_sub_cor_handle;

                Awaiter(const Awaiter&) = delete;
                Awaiter& operator=(const Awaiter& other) = delete;

                Awaiter()
                {

                }

                Awaiter(Awaiter&& other)
                    : m_sub_cor_handle(other.m_sub_cor_handle)
                {
                    other.m_sub_cor_handle = nullptr;
                }
                T await_resume() const 
                {
                    if constexpr (requires { requires !std::is_void_v<T>; }) 
                    {
                        return m_sub_cor_handle.promise().getReturnValue();
                    }
                    else
                    {
                        return;
                    }
                }
                ~Awaiter()
                {
                    if(m_sub_cor_handle != nullptr)
                        m_sub_cor_handle.destroy();
                }
            };

            typename Core::Coroutine::CorHandle<T>::StdHandle cor_handle = yield_for_sub_coroutine.m_sub_coroutine.dumpHandle();
            Awaiter ret_awaiter;
            ret_awaiter.m_sub_cor_handle = cor_handle;

            m_update_fun = [cor_handle = std::move(cor_handle)](Task& running_task) mutable
            {
                CorHandle<T>::resume(cor_handle, running_task);
                if(cor_handle.done())
                {
                    return true;
                }
                return false;
            };

            m_op_flag = (OpFlag)((std::uint32_t)m_op_flag | (std::uint32_t)OpFlag::UPDATE_IMMEDIATELY);
            return ret_awaiter;
        }

        
        template<typename T>
        auto yield_value(CreateParallelCoroutine<T>&& yield_create_parallel_coroutine)
        {
            typename Core::Coroutine::CorHandle<T>::StdHandle cor_handle = yield_create_parallel_coroutine.m_parallel_coroutine.dumpHandle();
            m_update_fun = [cor_handle = std::move(cor_handle)](Task& running_task) mutable
            {   
                running_task.appendCoroutine(
                    Core::Coroutine::CorHandle<T>(std::move(cor_handle))
                );
                return true;
            };

            m_op_flag = (OpFlag)((std::uint32_t)m_op_flag | (std::uint32_t)OpFlag::UPDATE_IMMEDIATELY);
            return std::suspend_always{};
        }

        bool update(Task& running_task)
        {
            if(m_update_fun != nullptr)
            {
                if(m_update_fun(running_task))
                {
                    m_update_fun = nullptr;
                }
                else
                {
                    return false;
                }
            }
            return true;            
        }
    };

    template<typename T>
    class Promise
        : public PromiseBase
    {
    protected:
        T m_ret_value{};
    public:
        Core::Coroutine::CorHandle<T> get_return_object() 
        { 
            return Core::Coroutine::CorHandle<T>::StdHandle::from_promise(*this); 
        }

        void return_value(T value)
        {
            m_ret_value = value;
            return;
        }

        constexpr T getReturnValue()
        {
            return m_ret_value; 
        }
    };

    template<>
    class Promise<void>
        : public PromiseBase
    {
    public:
        CorHandle<void> get_return_object() 
        { 
            return {CorHandle<void>::StdHandle::from_promise(*this)}; 
        }

        void return_void()
        {
            return;
        }
    };
}