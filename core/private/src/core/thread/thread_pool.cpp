#include "base/prerequisites.h"
#include "core/core.h"
#include "core/thread/thread_pool.h"

namespace Arieo::Core
{
    thread_local size_t tls_cur_thread_id = 0;

    void ThreadPool::start(size_t num_threads)
    {
        std::unique_lock<std::mutex> lock(m_mutex_for_worker);
        for(size_t i_thread = 0; i_thread < num_threads; i_thread++)
        {
            m_workers.emplace_back(
                [this, i_thread] 
                {
                    tls_cur_thread_id = i_thread + 1;
                    while(true)
                    {
                        std::function<void()> new_task;
                        {
                            //spin for some time
                            std::size_t remain_spin_count = 100;
                            while(remain_spin_count > 0)
                            {
                                if(m_task_queue.try_dequeue(new_task))
                                {
                                    break;
                                }
                                std::this_thread::yield();
                                remain_spin_count--;
                            }

                            // not got new task in spin
                            if(remain_spin_count == 0)
                            {
                                std::unique_lock<std::mutex> lock(m_mutex_for_worker);
                                
                                m_sleeping_thread_counter.fetch_add(1);
                                m_cv_for_worker.wait(lock, [this, &new_task] 
                                {   
                                    m_sleeping_thread_counter.fetch_sub(1);
                                    if(m_task_queue.try_dequeue(new_task) || m_stoped)
                                    {
                                        return true;
                                    }

                                    m_sleeping_thread_counter.fetch_add(1);
                                    return false;
                                });
                            }
                        }

                        if (m_stoped)
                        {
                            return;
                        }

                        new_task();
                    }
                }
            );
        }
        m_stoped = false;
    }

    void ThreadPool::stop()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex_for_worker);
            m_stoped = true;
        }

        for(std::thread& worker : m_workers)
        {
            m_cv_for_worker.notify_one();
        }
        for(std::thread& worker : m_workers)
        {
            worker.join();
        }
        m_workers.clear();
        m_stoped = false;
    }

    size_t ThreadPool::getThreadNum()
    {
        return m_workers.size();
    }

    size_t ThreadPool::getPendingTaskNum()
    {
        return m_task_queue.size_approx();
    }
 
    size_t ThreadPool::getCurrentThreadId()
    {
        return tls_cur_thread_id;
    }
}

