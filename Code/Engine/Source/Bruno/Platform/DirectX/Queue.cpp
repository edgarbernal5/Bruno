#include "brpch.h"
#include "Queue.h"

#ifdef BR_DEBUG
#include <dxgidebug.h>
#endif

#include "D3DHelpers.h"

#include <numeric>

//extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION; }
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

namespace Bruno::DX
{
    CommandQueue::CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
        : m_fenceValue(0)
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));
        
        // 1. Crear la valla con valor inicial 0
        ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        
        m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_fenceEvent)
        {
            throw std::runtime_error("Fallo al crear el evento de sincronización de Windows.");
        }

        // CREACIÓN DE MÚLTIPLES ALLOCATORS
        for (uint32_t i = 0; i < BufferCount; ++i) {
            ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocators[i])));
            m_frameFenceValues[i] = 0; // Inicializamos los tickets
        }

        // Creamos la CommandList usando el primer allocator
        ThrowIfFailed(device->CreateCommandList(
            0, type, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList)
        ));

        m_commandList->Close();
    }

    CommandQueue::~CommandQueue()
    {
        Flush();
        ::CloseHandle(m_fenceEvent);
    }

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandQueue::GetCommandList(uint32_t frameIndex)
    {
        // 1. ANTES de usar el Allocator de este frame, verificamos que la GPU ya terminó de leerlo
        WaitForFenceValue(m_frameFenceValues[frameIndex]);

        // 2. Ahora es 100% seguro resetearlo
        auto& currentAllocator = m_commandAllocators[frameIndex];
        ThrowIfFailed(currentAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(currentAllocator.Get(), nullptr));
        
        return m_commandList;
    }

    uint64_t CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, uint32_t frameIndex)
    {
        ThrowIfFailed(commandList->Close());

        ID3D12CommandList* const ppCommandLists[] = { commandList.Get() };
        m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

        // Aumentamos el ticket global
        m_fenceValue++;
        ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));

        // GUARDAMOS EL TICKET ESPECÍFICO DE ESTE FRAME
        m_frameFenceValues[frameIndex] = m_fenceValue;

        return m_fenceValue;
    }

    void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
    {
        // Verificamos si la GPU ya cruzó la valla (Si es así, la CPU ni se frena)
        if (m_fence->GetCompletedValue() < fenceValue) 
        {
            // Le decimos a la GPU: "Despierta a este evento de Windows cuando llegues a este valor"
            ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
            
            // Dormimos el hilo de la CPU (Cero uso de CPU) hasta que la GPU grite "¡Terminé!"
            ::WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    void CommandQueue::Flush()
    {
        // Truco maestro: Metemos una señal vacía y obligamos a la CPU a esperarla.
        // Garantiza que TODAS las operaciones previas de la GPU hayan concluido.
        m_fenceValue++;
        ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
        WaitForFenceValue(m_fenceValue);
    }
}