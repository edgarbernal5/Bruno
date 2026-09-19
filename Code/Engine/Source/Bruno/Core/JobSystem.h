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

        // Dispatch Masivo (Data-Oriented Map-Reduce)
        // Divide una tarea grande (ej: 10,000 entidades) en "Chunks" para distribuirla en múltiples hilos
        void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(uint32_t start, uint32_t end)>& job, JobDispatchGroup* group);
        
        // Esperar a que todo termine (Sincronización en el Main Thread)
        void Wait(const JobDispatchGroup& group);
        
        static JobSystem& Get()
        {
            static JobSystem instance;
            return instance;
        }
    private:
        // Función interna para extraer un job de forma segura y rápida
        bool TryPop(std::function<void()>& outJob);
        
        std::vector<std::thread> m_workers;
        std::queue<std::function<void()>> m_jobQueue;
        
        std::mutex m_queueMutex;
        std::condition_variable m_condition;
        
        std::atomic<bool> m_stop{ false };
    };
}