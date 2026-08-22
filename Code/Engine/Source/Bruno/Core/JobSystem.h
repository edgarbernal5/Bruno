#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
#include <queue>
#include <atomic>

namespace Bruno
{
    struct JobDispatchGroup
    {
        std::atomic<uint32_t> pendingJobs{ 0 };
    };
    
    class JobSystem
    {
    public:
        void Initialize();
        void Shutdown();

        // Añadir una tarea individual
        void Execute(std::function<void()> job, JobDispatchGroup* group = nullptr);

        // Esperar a que todo termine (Sincronización en el Main Thread)
        void Wait(const JobDispatchGroup& group);
        
        static JobSystem& Get()
        {
            static JobSystem instance;
            return instance;
        }
    private:
        std::vector<std::thread> m_workers;
        std::queue<std::function<void()>> m_jobQueue;
        
        std::mutex m_queueMutex;
        std::condition_variable m_condition;
        
        std::atomic<bool> m_stop{ false };
    };
}