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
        ~CommandQueueManager(); // Importante para limpiar el evento
        
        // Getters que devuelven tu abstracción, no punteros crudos
        CommandQueue& GetGraphicsQueue() { return *m_directQueue; }
        CommandQueue& GetCopyQueue() { return *m_copyQueue; }

        // Ejecuta el UploadContext en la cola de copia y devuelve el Fence Value a esperar
        uint64_t ExecuteAndReturnFence(UploadContext& uploadContext);

        // Bloquea el hilo de la CPU hasta que el Fence alcance el valor especificado
        void WaitForGpuFence(uint64_t fenceValue);

        static std::shared_ptr<CommandQueueManager> Create(GraphicsDevice& device);
        
    private:
        GraphicsDevice& m_device;

        // --- MIEMBROS DE SINCRONIZACIÓN ---
        
        // ¡CRÍTICO! Usar ComPtr para el Fence nativo
        Microsoft::WRL::ComPtr<ID3D12Fence> m_uploadFence;
        
        // Tracks el valor a señalizar *siguiente*
        uint64_t m_nextFenceValue{ 1 }; 
        
        // Evento de Windows para esperas CPU-side. *KISS: manual CloseHandle*
        HANDLE m_fenceEvent{ nullptr }; 

        // --- LAS COLAS NATIVAS ABSTRAÍDAS ---
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