#include "brpch.h"
#include "JobSystem.h"

namespace Bruno
{
    void JobSystem::Initialize()
    {
        // Obtenemos los cores reales del procesador (dejamos 1 para el Main Thread / OS)
        uint32_t numThreads = std::thread::hardware_concurrency() - 1;
        if (numThreads == 0)
        {
            numThreads = 1;
        }
        
        for (uint32_t i = 0; i < numThreads; ++i)
        {
            m_workers.emplace_back([this]()
            {
                while (true)
                {
                    std::function<void()> job;
                    {
                        std::unique_lock lock(m_queueMutex);
                        m_condition.wait(lock, [this]
                        { 
                            return m_stop.load() || !m_jobQueue.empty(); 
                        });

                        if (m_stop.load() && m_jobQueue.empty())
                        {
                            return;
                        }
                        
                        job = std::move(m_jobQueue.front());
                        m_jobQueue.pop();
                    }

                    // Ejecutar el trabajo
                    job();
                }
            });
        }
    }

    void JobSystem::Shutdown()
    {
        m_stop.store(true);
        m_condition.notify_all();
        for (std::thread& worker : m_workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void JobSystem::Execute(std::function<void()> job, JobDispatchGroup* group)
    {
        if (group)
        {
            // Incrementamos el contador ANTES de encolar
            group->pendingJobs.fetch_add(1, std::memory_order_acquire);
        }

        {
            std::scoped_lock lock(m_queueMutex);
            // Si pasamos un grupo, envolvemos el job original para que reduzca el contador al terminar
            if (group)
            {
                m_jobQueue.push([job, group]()
                {
                    job(); // Ejecuta la tarea real
                    group->pendingJobs.fetch_sub(1, std::memory_order_release);
                });
            }
            else
            {
                m_jobQueue.push(std::move(job));
            }
        }
        m_condition.notify_one();
    }

    void JobSystem::Wait(const JobDispatchGroup& group)
    {
        // Spin-lock que cede la CPU mientras espera que ESTE grupo termine
        while (group.pendingJobs.load(std::memory_order_acquire) > 0)
        {
            std::this_thread::yield(); 
        }
    }
}
