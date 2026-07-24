#include "brpch.h"
#include "CommandQueueManager.h"

#include "GraphicsDevice.h" // Incluye para acceder al dispositivo nativo
#include "CommandQueue.h"
#include "UploadContext.h"

namespace Bruno
{
    CommandQueueManager::CommandQueueManager(GraphicsDevice& device) 
        : m_device(device)
    {
        // 1. INSTANCIAMOS TUS ABSTRACCIONES DE COLAS
        // Nota: Asegúrate de que tu constructor de CommandQueue acepte el tipo de cola.
        m_directQueue = std::make_unique<CommandQueue>(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_copyQueue = std::make_unique<CommandQueue>(device, D3D12_COMMAND_LIST_TYPE_COPY);

        // 2. CREAMOS LOS OBJETOS DE SINCRONIZACIÓN PARA CARGA (DMA)
        ThrowIfFailed(device.GetNativeDevice()->CreateFence(
            0, 
            D3D12_FENCE_FLAG_NONE, 
            IID_PPV_ARGS(&m_uploadFence)
        ));

        m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    CommandQueueManager::~CommandQueueManager()
    {
        if (m_fenceEvent != nullptr)
        {
            CloseHandle(m_fenceEvent);
            m_fenceEvent = nullptr;
        }
    }

    uint64_t CommandQueueManager::ExecuteAndReturnFence(UploadContext& uploadContext)
    {
        // 1. Cerramos el contexto para que esté listo para ejecutarse
        uploadContext.Close();

        // 2. Obtenemos la lista nativa del contexto
        ID3D12CommandList* ppCommandLists[] = { uploadContext.GetNative() };

        // 3. Ejecutamos EN LA COLA DE COPIA (DMA)
        // Necesitarás exponer el método nativo ExecuteCommandLists en tu abstracción CommandQueue
        m_copyQueue->GetNativeQueue()->ExecuteCommandLists(1, ppCommandLists);

        // 4. Señalizamos el Fence
        uint64_t fenceValueToWait = m_nextFenceValue;
        ThrowIfFailed(m_copyQueue->GetNativeQueue()->Signal(m_uploadFence.Get(), fenceValueToWait));

        m_nextFenceValue++;
        return fenceValueToWait;
    }

    void CommandQueueManager::WaitForGpuFence(uint64_t fenceValue)
    {
        // Verificamos si la GPU ya pasó por este punto
        if (m_uploadFence->GetCompletedValue() < fenceValue)
        {
            // Le pedimos a Windows que active el evento cuando la GPU alcance el valor
            ThrowIfFailed(m_uploadFence->SetEventOnCompletion(fenceValue, m_fenceEvent));
            
            // Dormimos el hilo de la CPU (Cero consumo de procesador mientras esperamos a la GPU)
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    std::shared_ptr<CommandQueueManager> CommandQueueManager::Create(GraphicsDevice& device)
    {
        return std::make_shared<CommandQueueManager>(device);
    }
}
