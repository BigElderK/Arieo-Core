#pragma once
namespace Arieo::Core
{
    class JobWorker final
    {
    protected:
        std::list<Core::Coroutine::Task> m_task_list;        
    public:
        void appendTask(Core::Coroutine::Task&& task)
        {
            m_task_list.emplace_back(std::move(task));                        
        }

        void updateTasks(std::function<bool(Coroutine::Task::Tasklet&)>&& preprocess_append_tasklet_fun)
        {
            for(auto task_iter = m_task_list.begin(); task_iter != m_task_list.end();)
            {
                if(task_iter->m_preprocess_append_tasklet_fun == nullptr)
                {
                    task_iter->m_preprocess_append_tasklet_fun = preprocess_append_tasklet_fun; 
                }

                task_iter->updateOneStep();
                if(task_iter->isFinished())
                {
                    task_iter = m_task_list.erase(task_iter);
                }
                else
                {
                    task_iter++;
                } 
            }
        }

        bool isEmpty()
        {
            return m_task_list.empty();
        }
    };
}