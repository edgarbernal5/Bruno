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
        // 1. Crear la Cola de Comandos
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));

        // 2. Crear la Valla (Fence) y el Evento de Windows
        ThrowIfFailed(device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_fenceEvent) {
            throw std::runtime_error("Fallo al crear el evento de sincronización de Windows.");
        }

        // 3. Crear Allocator y CommandList
        ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocator)));
        ThrowIfFailed(device->CreateCommandList(
            0, type, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)
        ));

        // En DX12, las CommandLists se crean en estado "grabando" (recording).
        // Las cerramos inmediatamente porque las abriremos en el ciclo de vida del frame.
        m_commandList->Close();
    }

    CommandQueue::~CommandQueue() {
        // KISS y Safety: Asegurarnos de que la GPU terminó todo antes de destruir nada
        Flush();
        ::CloseHandle(m_fenceEvent);
    }

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandQueue::GetCommandList() {
        // Resetear el Allocator y la CommandList para empezar a grabar comandos
        ThrowIfFailed(m_commandAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
        
        return m_commandList;
    }

    uint64_t CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList) {
        ThrowIfFailed(commandList->Close());

        // Enviar la lista a la GPU
        ID3D12CommandList* const ppCommandLists[] = { commandList.Get() };
        m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

        // Incrementar el ticket y ordenarle a la GPU que actualice la valla cuando termine
        m_fenceValue++;
        ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));

        return m_fenceValue;
    }

    void CommandQueue::WaitForFenceValue(uint64_t fenceValue) {
        // Si la GPU ya cruzó o alcanzó el valor de la valla, no hacemos nada (la CPU no se bloquea)
        if (m_fence->GetCompletedValue() < fenceValue) {
            
            // Le pedimos a la valla que dispare el evento de Windows cuando alcance el valor
            ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
            
            // Dormimos el hilo de la CPU (0% uso de CPU) hasta que la GPU despierte el evento
            ::WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    void CommandQueue::Flush() {
        // Enviar una señal vacía a la cola y forzar a la CPU a esperar a que la GPU la alcance.
        // Esto garantiza que TODO lo anterior en la cola haya terminado.
        m_fenceValue++;
        ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
        WaitForFenceValue(m_fenceValue);
    }
}