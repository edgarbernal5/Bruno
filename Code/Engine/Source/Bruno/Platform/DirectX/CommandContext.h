#pragma once

#include <array>
#include "D3DConstants.h"

#include "D3DHelpers.h"

namespace Bruno::DX
{
    class CommandContext
    {
    public:
    
        // Getter para métodos que aún no hayas envuelto
        ID3D12GraphicsCommandList* GetNative() const { return m_commandList.Get(); }
        
        // Métodos de ciclo de vida esenciales
        void Close();
        void Reset();
        
    protected:
        // Constructor 1 (El que ya tienes, usado por UploadContext para crear sus propias listas)
        CommandContext(DX::GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType);
        
        // Constructor 2 (NUEVO: Usado por GraphicsContext para envolver la lista del RenderLoop)
        CommandContext(DX::GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType, ID3D12GraphicsCommandList* existingList, ID3D12CommandAllocator* existingAllocator);
        
        DX::GraphicsDevice& m_device;
        D3D12_COMMAND_LIST_TYPE m_commandType;
        
        // Usar ComPtr para que manejen su propio conteo de referencias (Release)
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    };
}