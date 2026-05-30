#include "brpch.h"
#include "RenderContext.h"

#include "Device.h"

namespace Bruno::DX
{
    RenderContext::RenderContext(GraphicsDevice& device) : m_device(device) 
    {
        auto nativeDevice = m_device.GetNativeDevice();

        ThrowIfFailed(nativeDevice->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            IID_PPV_ARGS(&m_commandAllocator)
        ));

        ThrowIfFailed(nativeDevice->CreateCommandList(
            0, 
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            m_commandAllocator.Get(), 
            nullptr, 
            IID_PPV_ARGS(&m_commandList)
        ));

        // DX12 crea las listas abiertas por defecto. La cerramos hasta que hagamos Begin().
        m_commandList->Close(); 
    }

    void RenderContext::Begin() 
    {
        // Reseteamos el allocador (La GPU ya debe haber terminado con estos comandos)
        // NOTA: Requiere sincronización con Fences que implementaremos después.
        ThrowIfFailed(m_commandAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
    }

    void RenderContext::Execute() 
    {
        ThrowIfFailed(m_commandList->Close());

        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_device.GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}
