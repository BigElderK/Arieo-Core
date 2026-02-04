#include <chrono>
#include <functional>
namespace Arieo::Base
{
    class ExecuteTimeGraud
    {
    protected:
        std::function<void(std::chrono::microseconds)> m_callback_function;
        decltype(std::chrono::high_resolution_clock::now()) m_begin_time_point;
    public:
        ExecuteTimeGraud(std::function<void(std::chrono::microseconds)> callback_func)
            : m_callback_function(callback_func)
        {
            m_begin_time_point = std::chrono::high_resolution_clock::now();
        }

        ~ExecuteTimeGraud()
        {
            auto m_end_time_point = std::chrono::high_resolution_clock::now(); 
            if(m_callback_function != nullptr)
            {
                m_callback_function(std::chrono::duration_cast<std::chrono::microseconds>(m_end_time_point - m_begin_time_point));
            }
        }
    };
}