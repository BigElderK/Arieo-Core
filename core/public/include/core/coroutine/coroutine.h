#pragma once
#include <coroutine>
namespace Arieo::Core::Coroutine
{
    class Task;

    template<typename T>
    class CorHandle;

    template<typename T>
    class Promise;

    template<typename T>
    class CorHandle
    {
    public:
        using promise_type = Promise<T>;
        using StdHandle = std::coroutine_handle<Promise<T>>;
    public:
        friend class PromiseBase;
        friend class Task;
    private:
        StdHandle m_handle;
        StdHandle dumpHandle()
        {
            StdHandle ret = m_handle;
            m_handle = nullptr;
            return ret;
        }

        static void resume(StdHandle& handle, Task& task)
        {
            promise_type& promise = handle.promise();
            if(promise.update(task))
            {
                while(true)
                {
                    if(handle.done())
                    {
                        break;
                    }
                    handle.resume();
                    promise = handle.promise();
                    if((uint32_t)promise.m_op_flag & (uint32_t)Promise<T>::OpFlag::UPDATE_IMMEDIATELY)
                    {
                        if(promise.update(task) == false)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                };
            }
        }
    public:
        CorHandle(const CorHandle& other) = delete;
        CorHandle& operator=(const CorHandle& other) = delete;

        CorHandle()
            : m_handle(nullptr)
        {

        }

        CorHandle(CorHandle&& other)
            : m_handle(other.m_handle) 
        {
            other.m_handle = nullptr;
        }

        CorHandle(StdHandle&& t) noexcept
            : m_handle(t)
        {

        }

        ~CorHandle()
        {
            destroy();
        }

        bool isNull()
        {
            return m_handle == nullptr;
        }

        void destroy()
        {
            if(m_handle != nullptr)
            {
                m_handle.destroy();
                m_handle = nullptr;
            }
        }

        CorHandle& operator=(CorHandle&& other)
        {
            m_handle = other.m_handle;
            other.m_handle = nullptr;
            return *this;
        }

        Promise<T>& getPromise()
        {
            return m_handle.promise();
        }

        bool isDone()
        {
            return m_handle.done();
        }
    };    


}