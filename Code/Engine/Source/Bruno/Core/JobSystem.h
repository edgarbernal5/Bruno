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
    class JobSystem
    {
    public:
        void Initialize();

        void Shutdown();

        // Añadir una tarea individual
        void Execute(std::function<void()> job);

        // Esperar a que todo termine (Sincronización en el Main Thread)
        void Wait();
        
    private:
        std::vector<std::thread> m_workers;
        std::queue<std::function<void()>> m_jobQueue;
        
        std::mutex m_queueMutex;
        std::condition_variable m_condition;
        
        std::atomic<bool> m_stop{ false };
        std::atomic<uint32_t> m_activeJobs{ 0 };
    };

    // Singleton o instancia global para fácil acceso
    extern JobSystem g_JobSystem;
}