
#include "concurrentqueue.h"
#include <atomic>
namespace Arieo::Base
{
    template<typename T>
    using ConcurrentQueue = moodycamel::ConcurrentQueue<T>;

    class ConcurrentUtiliy
    {
    public:
        template<typename T>
        static bool consumeOneIfNotZero(std::atomic<T>& value)
        {
            T expected = value.load(std::memory_order_relaxed);
            do
            {
                if(expected == 0)
                {
                    return false;
                }
            } while (
                value.compare_exchange_weak(
                    expected, 
                    expected -1,
                    std::memory_order_release, std::memory_order_relaxed
                )
            );
            return true;
        }
    };
}