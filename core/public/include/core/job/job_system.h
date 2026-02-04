#pragma once
#include "core/singleton/singleton.h"
#include "core/thread/thread_pool.h"
#include "core/coroutine/coroutine.h"
#include "core/coroutine/task.h"
#include "core/job/job_worker.h"
#include <future>
namespace Arieo::Core
{
    class JobSystem
    {
    public:
        JobSystem();
        ~JobSystem();

        void enqueueTask(Arieo::Core::Coroutine::Task&& task);
        std::future<void> updateOneFrame(Core::ThreadPool& thread_pool, size_t thread_count);  
        void updateOneFrame();
        bool isUpdateFinished();
        size_t getTaskCount();
    private:
        std::atomic<size_t> m_update_finish_wait_count = 0;
        std::promise<void> m_update_finished_promise;

        Base::ConcurrentQueue<Core::Coroutine::Task> m_task_queue_1;        
        Base::ConcurrentQueue<Core::Coroutine::Task> m_task_queue_2;        
        std::reference_wrapper<Base::ConcurrentQueue<Core::Coroutine::Task>> m_task_queue_front;        
        std::reference_wrapper<Base::ConcurrentQueue<Core::Coroutine::Task>> m_task_queue_back;
    };
}