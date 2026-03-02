#pragma once
#include "core/coroutine/coroutine.h"

namespace Arieo::Core
{
    class JobSystem;
}

namespace Arieo::Core::Coroutine
{
    class PromiseBase;
    class Task; // forward declaration for ITaskletFunc

    // DLL-safe abstract callable — concrete implementations are compiled
    // in the caller's DLL; destruction crosses back via the RefControlBlock
    // delete callback (a plain function pointer), which is safe.
    class ITaskletFunc
    {
    public:
        virtual bool execute(Task& task) = 0;
        virtual ~ITaskletFunc() = default;
    };

    class IPreprocessTaskletDelegate; // defined after Task (needs the nested Task::Tasklet type)

    class Task
    {
    public:
        class Tasklet final
        {
        public:
            Tasklet() {}
            explicit Tasklet(Base::Interop::SharedRef<ITaskletFunc>&& func)
                : m_task_fun(std::move(func)) {}
        private:
            friend class Task;
            Base::Interop::SharedRef<ITaskletFunc> m_task_fun;
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
            appendTasklet(std::move(tasklet));
        }

        Task& operator=(Task&& other)
        {
            m_tasklet_list = std::move(other.m_tasklet_list);
            return *this;
        }

        template<typename T>
        static Tasklet generatorTasklet(Core::Coroutine::CorHandle<T>&& startup_cor)
        {
            using StdHandle = typename Core::Coroutine::CorHandle<T>::StdHandle;
            StdHandle cor_handle = startup_cor.dumpHandle();

            // Concrete implementation compiled in the CALLER'S DLL.
            // The delete callback stored in RefControlBlock is a plain function
            // pointer — safe to call from any DLL.
            struct CoroutineTaskletFunc final : ITaskletFunc
            {
                StdHandle handle;
                explicit CoroutineTaskletFunc(StdHandle&& h) : handle(std::move(h)) {}
                ~CoroutineTaskletFunc() override
                {
                    if (handle) { handle.destroy(); }
                }
                bool execute(Task& task) override
                {
                    Core::Coroutine::CorHandle<T>::resume(handle, task);
                    if (handle.done())
                    {
                        handle.destroy();
                        handle = StdHandle{}; // prevent double-destroy in dtor
                        return true;
                    }
                    return false;
                }
            };

            return Tasklet(Base::Interop::SharedRef<ITaskletFunc>::createInstance<CoroutineTaskletFunc>(std::move(cor_handle)));
        }

        template<typename T>
        void appendCoroutine(Core::Coroutine::CorHandle<T>&& startup_cor)
        {
            appendTasklet(
                generatorTasklet(std::move(startup_cor))
            );
        }

        void appendTasklet(Tasklet&& tasklet); // body in task.cpp — keeps emplace_back in Core's heap

        Base::Interop::SharedRef<IPreprocessTaskletDelegate> m_preprocess_append_tasklet_delegate;

        bool isFinished();
        void updateOneStep();

        Task(Task&) = delete;
        Task& operator=(Task&) = delete;
    };

    // Defined here (after Task) so it can reference the nested Task::Tasklet type.
    class IPreprocessTaskletDelegate
    {
    public:
        virtual bool preprocess(Task::Tasklet& tasklet) = 0;
        virtual ~IPreprocessTaskletDelegate() = default;
    };
};




