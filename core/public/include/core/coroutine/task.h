#pragma once
#include "core/coroutine/coroutine.h"
namespace Arieo::Core
{
    class JobSystem;
}

namespace Arieo::Core::Coroutine
{
    class PromiseBase;
    class Task
    {
    public:
        class Tasklet final
        {
        public:
            Tasklet(){}
            Tasklet(std::function<bool(Task&)>&& task_func)
                : m_task_fun(std::move(task_func))
            {
                
            }
        private:
            friend class Task;
            std::function<bool(Task&)> m_task_fun;
        };
    private:
        friend class Core::JobSystem;
    public:
        std::list<Tasklet> m_tasklet_list;
    public:
        Task()
        {

        }

        ~Task()
        {

        }

        template<typename T>
        Task(Core::Coroutine::CorHandle<T>&& startup_cor)
        {
            appendCoroutine(std::move(startup_cor));
        }

        Task(Task&& other)
            : m_tasklet_list(std::move(other.m_tasklet_list)) 
        {

        }

        Task(Tasklet&& tasklet)
        {
            m_tasklet_list.emplace_back(std::move(tasklet));
        }

        Task& operator=(Task&& other)
        {
            m_tasklet_list = std::move(other.m_tasklet_list);
            return *this;
        }

        template<typename T>
        static Tasklet generatorTasklet(Core::Coroutine::CorHandle<T>&& startup_cor)
        {
            typename Core::Coroutine::CorHandle<T>::StdHandle cor_handle = startup_cor.dumpHandle();
            return Tasklet([cor_handle = std::move(cor_handle)](Task& task) mutable
            {
                Core::Coroutine::CorHandle<T>::resume(cor_handle, task);
                if(cor_handle.done())
                {
                    cor_handle.destroy();
                    return true;
                }
                return false;
            });
        }

        template<typename T>
        void appendCoroutine(Core::Coroutine::CorHandle<T>&& startup_cor)
        {
            appendTasklet(
                generatorTasklet(std::move(startup_cor))
            );
        }

        void appendTasklet(Core::Coroutine::Task::Tasklet&& tasklet)
        {
            if(m_preprocess_append_tasklet_fun != nullptr)
            {
                if(m_preprocess_append_tasklet_fun(tasklet))
                {
                    return;
                }
            }

            m_tasklet_list.emplace_back(
                std::move(tasklet)
            );
        }

        std::function<bool(Tasklet&)> m_preprocess_append_tasklet_fun = nullptr;

        bool isFinished();
        void updateOneStep();

        Task(Task&) = delete;
        Task& operator=(Task&) = delete;
    };
};