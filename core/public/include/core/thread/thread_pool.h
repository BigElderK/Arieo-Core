#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
namespace Arieo::Core
{
    class ThreadPool
    {
    private:
        std::mutex m_mutex_for_worker;
        std::condition_variable m_cv_for_worker;
        
        std::vector<std::thread> m_workers;
        bool m_stoped = false;
        Base::ConcurrentQueue<std::function<void()>> m_task_queue;

        std::atomic<std::uint32_t> m_sleeping_thread_counter;
    public:
        ThreadPool()
            : m_sleeping_thread_counter(0)
        {

        }

        ~ThreadPool()
        {
            stop();
        }

        void start(size_t num_threads = std::thread::hardware_concurrency());
        void stop();
        static size_t getCurrentThreadId();

        size_t getPendingTaskNum();
        size_t getThreadNum();

        template<class F, class... Args>
        auto enqueueTask(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> 
        {   
            using return_type = typename std::invoke_result_t<F, Args...>;
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
                
            std::future<return_type> res = task->get_future();
            {
                while(m_task_queue.enqueue(
                    [task]() 
                    {
                        (*task)(); 
                    }
                ) == false) 
                {
                    std::this_thread::yield();
                }
            }

            if(m_sleeping_thread_counter.load() != 0)
            {
                m_cv_for_worker.notify_one();    
            }
            
            return res;
        }
    };
}