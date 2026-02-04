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
            if(tasklet_iter->m_task_fun(*this) == true)
            {
                tasklet_iter = m_tasklet_list.erase(tasklet_iter);
            }
            else
            {
                tasklet_iter++;
            }
        }
    }
}
