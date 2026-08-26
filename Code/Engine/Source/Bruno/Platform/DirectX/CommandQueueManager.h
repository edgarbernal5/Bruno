#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

namespace Bruno
{
    class GraphicsDevice;
    class UploadContext;
    class CommandQueue;

    class CommandQueueManager
    {
    public:
        CommandQueueManager(GraphicsDevice& device);
        ~CommandQueueManager();
        
        CommandQueue& GetGraphicsQueue() { return *m_directQueue; }
        CommandQueue& GetCopyQueue() { return *m_copyQueue; }

        // Ejecuta el UploadContext en la cola de copia y devuelve el Fence Value a esperar
        uint64_t ExecuteAndReturnFence(UploadContext& uploadContext);

        // Bloquea el hilo de la CPU hasta que el Fence alcance el valor especificado
        void WaitForGpuFence(uint64_t fenceValue);

        static std::shared_ptr<CommandQueueManager> Create(GraphicsDevice& device);
        
    private:
        GraphicsDevice& m_device;
        
        Microsoft::WRL::ComPtr<ID3D12Fence> m_uploadFence;
        
        // Tracks el valor a señalizar *siguiente*
        uint64_t m_nextFenceValue{ 1 }; 
        
        HANDLE m_fenceEvent{ nullptr }; 

        std::unique_ptr<CommandQueue> m_directQueue;
        std::unique_ptr<CommandQueue> m_copyQueue;
    };
}

namespace Bruno::Graphics
{
    inline CommandQueueManager*& GetCommandQueueManager()
    {
        static CommandQueueManager* g_commandManager = nullptr;
        return g_commandManager;
    }
}