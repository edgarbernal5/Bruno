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
                    
                    // Esperar a que haya trabajo
                    {
                        std::unique_lock<std::mutex> lock(m_queueMutex);
                        m_condition.wait(lock, [this]
                        { 
                            return m_stop.load(std::memory_order_acquire) || !m_jobQueue.empty(); 
                        });

                        if (m_stop.load(std::memory_order_acquire) && m_jobQueue.empty())
                        {
                            return;
                        }
                        
                        job = std::move(m_jobQueue.front());
                        m_jobQueue.pop();
                    }
                    
                    // Ejecutar fuera del lock
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
            group->pendingJobs.fetch_add(1, std::memory_order_relaxed);
        }

        {
            std::scoped_lock lock(m_queueMutex);
            if (group) {
                m_jobQueue.emplace([job = std::move(job), group]()
                {
                    job();
                    // Release semánticamente asegura que la memoria se publique al terminar
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

    void JobSystem::Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(uint32_t start, uint32_t end)> &job, JobDispatchGroup* group)
    {
        if (jobCount == 0)
        {
            return;
        }
        
        uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;
        
        if (group)
        {
            group->pendingJobs.fetch_add(groupCount, std::memory_order_relaxed);
        }

        {
            std::scoped_lock lock(m_queueMutex);
            for (uint32_t i = 0; i < groupCount; ++i)
            {
                uint32_t start = i * groupSize;
                uint32_t end = std::min<uint32_t>(start + groupSize, jobCount);

                m_jobQueue.emplace([job, start, end, group]()
                {
                    job(start, end);
                    if (group)
                    {
                        group->pendingJobs.fetch_sub(1, std::memory_order_release);
                    }
                });
            }
        }
        
        // Despertamos a todos los hilos posibles para este trabajo masivo
        m_condition.notify_all();
    }

    void JobSystem::Wait(const JobDispatchGroup& group)
    {
        // Mientras haya tareas en este grupo...
        while (group.pendingJobs.load(std::memory_order_acquire) > 0)
        {
            std::function<void()> job;
            // ... el hilo principal roba una tarea de la cola general y la ejecuta
            if (TryPop(job))
            {
                job();
            }
            else 
            {
                // Si la cola está vacía pero el grupo no ha terminado, cedemos ciclos
                std::this_thread::yield();
            }
        }
    }

    bool JobSystem::TryPop(std::function<void()>& outJob)
    {
        std::scoped_lock lock(m_queueMutex);
        if (m_jobQueue.empty() || m_stop.load(std::memory_order_relaxed))
        {
            return false;
        }
        outJob = std::move(m_jobQueue.front());
        m_jobQueue.pop();
        return true;
    }
}
