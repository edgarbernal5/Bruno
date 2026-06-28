// FrameActionQueue.h
#include <functional>
#include <vector>
#include <mutex>

namespace Bruno
{
    class FrameActionQueue
    {
    public:
        static FrameActionQueue& Get()
        {
            static FrameActionQueue instance;
            return instance;
        }

        // Cualquier sistema puede encolar trabajo aquí
        void Enqueue(std::function<void()> action)
        {
            // Mutex es opcional si garantizas que todo pasa en el Main Thread,
            // pero es buena práctica prepararse para Multithreading.
            std::lock_guard<std::mutex> lock(m_mutex); 
            m_actions.push_back(action);
        }

        // Se llama 1 sola vez por frame
        void ExecuteAll()
        {
            std::vector<std::function<void()>> tempActions;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                tempActions = std::move(m_actions);
                // m_actions ahora está vacío y listo para el siguiente frame
            }

            for (auto& action : tempActions)
            {
                action();
            }
        }

    private:
        std::vector<std::function<void()>> m_actions;
        std::mutex m_mutex;
    };
}
