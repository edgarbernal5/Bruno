#pragma once

#include <array>
#include "D3DConstants.h"

#include "D3DHelpers.h"

namespace Bruno
{
    class CommandContext
    {
    public:
        ID3D12GraphicsCommandList* GetNative() const { return m_commandList.Get(); }
        
        void Close();
        void Reset();
        
    protected:
        CommandContext(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType);
        CommandContext(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType, ID3D12GraphicsCommandList* existingList, ID3D12CommandAllocator* existingAllocator);
        
        GraphicsDevice& m_device;
        D3D12_COMMAND_LIST_TYPE m_commandType;
        
        // Usar ComPtr para que manejen su propio conteo de referencias (Release)
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    };
}