#include "base/prerequisites.h"
#include "core/core.h"
#include "core/job/job_system.h"

namespace Arieo::Core
{
    JobSystem::JobSystem() :
        m_task_queue_front(m_task_queue_1),
        m_task_queue_back(m_task_queue_2)
    {

    }

    JobSystem::~JobSystem()
    {
    }

    void JobSystem::updateOneFrame()
    {
        if(isUpdateFinished() == false)
        {
            Core::Logger::fatal("job system last update not finished yet.");
            return;
        }

        Base::ConcurrentQueue<Core::Coroutine::Task>& running_task_queue_temp = m_task_queue_front;
        m_task_queue_front = m_task_queue_back;
        m_task_queue_back = running_task_queue_temp;

        Base::ConcurrentQueue<Core::Coroutine::Task>& task_queue_front = m_task_queue_front;
        Base::ConcurrentQueue<Core::Coroutine::Task>& task_queue_back = m_task_queue_back;

        while(true)
        {
            Coroutine::Task task;
            while(task_queue_front.try_dequeue(task) == false)
            {
                if(task_queue_front.size_approx() == 0)
                {
                    return;
                }
            }

            task.m_preprocess_append_tasklet_fun = 
                [this, &task_queue_back](Core::Coroutine::Task::Tasklet& tasklet) -> bool
                {
                    if(task_queue_back.enqueue(std::move(tasklet)) == false)
                    {
                        std::this_thread::yield();
                    } 
                    return true;
                };
                
            task.updateOneStep();
            if(task.isFinished() == false)
            {
                while(task_queue_back.enqueue(std::move(task)) == false)
                {
                    std::this_thread::yield();
                } 
            }
        }
    }

    std::future<void> JobSystem::updateOneFrame(Core::ThreadPool& thread_pool, size_t thread_count)
    {
        if(isUpdateFinished() == false)
        {
            Core::Logger::fatal("job system last update not finished yet.");
            return {};
        }

        Base::ConcurrentQueue<Core::Coroutine::Task>& running_task_queue_temp = m_task_queue_front;
        m_task_queue_front = m_task_queue_back;
        m_task_queue_back = running_task_queue_temp;
       
        m_update_finished_promise = std::move(std::promise<void>()); 
        m_update_finish_wait_count.store(thread_count);

        for(size_t i = 0; i < thread_count; i++)
        {
            thread_pool.enqueueTask(
                [this]()
                {
                    Base::ConcurrentQueue<Core::Coroutine::Task>& task_queue_front = m_task_queue_front;
                    Base::ConcurrentQueue<Core::Coroutine::Task>& task_queue_back = m_task_queue_back;

                    while(true)
                    {
                        Coroutine::Task task;
                        while(task_queue_front.try_dequeue(task) == false)
                        {
                            if(task_queue_front.size_approx() == 0)
                            {
                                size_t wait_count = m_update_finish_wait_count.fetch_sub(1); 
                                if(wait_count == 1)
                                {
                                    m_update_finished_promise.set_value();
                                }
                                return;
                            }
                        }

                        task.m_preprocess_append_tasklet_fun = 
                            [this, &task_queue_back](Core::Coroutine::Task::Tasklet& tasklet) -> bool
                            {
                                if(task_queue_back.enqueue(std::move(tasklet)) == false)
                                {
                                    std::this_thread::yield();
                                } 
                                return true;
                            };
                            
                        task.updateOneStep();
                        if(task.isFinished() == false)
                        {
                            while(task_queue_back.enqueue(std::move(task)) == false)
                            {
                                std::this_thread::yield();
                            } 
                        }
                    }
                } 
            );
        }
        return m_update_finished_promise.get_future();
    }

    void JobSystem::enqueueTask(Core::Coroutine::Task&& task)
    {
        m_task_queue_back.get().enqueue(std::move(task));
    }

    bool JobSystem::isUpdateFinished()
    {
        return m_update_finish_wait_count.load() == 0;
    }

    size_t JobSystem::getTaskCount()
    {
        return m_task_queue_back.get().size_approx();
    }
}