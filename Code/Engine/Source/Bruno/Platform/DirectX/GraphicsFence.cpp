#include "brpch.h"
#include "GraphicsFence.h"

#include "Device.h"

namespace Bruno::DX
{
    GraphicsFence::GraphicsFence(GraphicsDevice& device) 
    {
        // 1. Creamos el Fence de DX12
        ThrowIfFailed(device.GetNativeDevice()->CreateFence(
            m_currentValue, 
            D3D12_FENCE_FLAG_NONE, 
            IID_PPV_ARGS(&m_fence)
        ));

        // 2. Creamos el Evento de Windows (Win32 API)
        m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr) 
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    GraphicsFence::~GraphicsFence() 
    {
        // RAII: Limpiamos el handle de Windows de forma segura
        if (m_fenceEvent) 
        {
            ::CloseHandle(m_fenceEvent);
            m_fenceEvent = nullptr;
        }
    }

    uint64_t GraphicsFence::Signal(ID3D12CommandQueue* commandQueue) 
    {
        m_currentValue++;
        
        // Ponemos la instrucción en la cola de la GPU. 
        // ¡Ojo! Esto no bloquea la CPU, solo encola la orden.
        ThrowIfFailed(commandQueue->Signal(m_fence.Get(), m_currentValue));
        
        return m_currentValue;
    }

    void GraphicsFence::WaitForValue(uint64_t expectedFenceValue) 
    {
        // Si la GPU ya pasó este valor, no hacemos nada (KISS - Fast Path)
        if (m_fence->GetCompletedValue() >= expectedFenceValue) 
        {
            return;
        }

        // Si la GPU aún no llega, le decimos a Windows que despierte a la CPU
        // a través del evento cuando la GPU alcance el valor.
        ThrowIfFailed(m_fence->SetEventOnCompletion(expectedFenceValue, m_fenceEvent));

        // Bloqueamos el hilo de la CPU. (Consumo de CPU baja a 0% mientras esperamos)
        ::WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    void GraphicsFence::Flush(ID3D12CommandQueue* commandQueue) 
    {
        uint64_t fenceValueToWaitFor = Signal(commandQueue);
        WaitForValue(fenceValueToWaitFor);
    }

}