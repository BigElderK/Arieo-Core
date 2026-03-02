#include "base/prerequisites.h"
#include "core/core.h"

#include "core/coroutine/task.h"
namespace Arieo::Core::Coroutine
{
    bool Task::isFinished()
    {
        return m_tasklet_list.empty();
    }

    void Task::updateOneStep()
    {
        for(auto tasklet_iter = m_tasklet_list.begin(); tasklet_iter != m_tasklet_list.end();) 
        {
            if(tasklet_iter->m_task_fun->execute(*this) == true)
            {
                tasklet_iter = m_tasklet_list.erase(tasklet_iter);
            }
            else
            {
                tasklet_iter++;
            }
        }
    }

    void Task::appendTasklet(Task::Tasklet&& tasklet)
    {
        if (m_preprocess_append_tasklet_delegate != nullptr)
        {
            if (m_preprocess_append_tasklet_delegate->preprocess(tasklet))
            {
                return;
            }
        }
        m_tasklet_list.emplace_back(std::move(tasklet));
    }
}





