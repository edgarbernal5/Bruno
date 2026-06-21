#include "brpch.h"
#include "CommandContext.h"

#include "Device.h"

namespace Bruno::DX
{
    void CommandContext::Close()
    {
        ThrowIfFailed(m_commandList->Close());
    }

    void CommandContext::Reset()
    {
        ThrowIfFailed(m_commandAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
    }

    CommandContext::CommandContext(DX::GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType) :
        m_device(device), m_commandType(commandType)
    {
        ThrowIfFailed(device.GetNativeDevice()->CreateCommandAllocator(
            commandType, IID_PPV_ARGS(&m_commandAllocator)));
        
        ThrowIfFailed(device.GetNativeDevice()->CreateCommandList(
            0, commandType, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
        
        // Forzamos a que el contexto nazca CERRADO. 
        // Así es obligatorio llamar a Reset() antes de usarlo por primera vez.
        m_commandList->Close();
    }

    CommandContext::CommandContext(DX::GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType,
        ID3D12GraphicsCommandList* existingList, ID3D12CommandAllocator* existingAllocator) :
        m_device(device), m_commandType(commandType), m_commandList(existingList), m_commandAllocator(existingAllocator)
    {
    }
}
