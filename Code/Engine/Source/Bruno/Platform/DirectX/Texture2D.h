#pragma once

#include "D3DHelpers.h"
#include "DescriptorAllocator_Gem.h"
#include <wrl/client.h>
#include <string>

#include "UploadHeap.h"


namespace Bruno::DX 
{
    class Texture2D 
    {
    public:
        // Carga una textura desde disco y reserva su lugar en el heap
        Texture2D(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, 
                  DescriptorAllocator& srvAllocator, const std::wstring& filePath);
        
        ~Texture2D() = default;

        // Necesario para el Render Loop
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_allocation.GPU; }
        [[nodiscard]] ID3D12Resource* GetResource() const { return m_textureResource.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_textureResource;
        std::unique_ptr<UploadHeap> m_uploadHeap; // Necesario para subir datos a VRAM
        DescriptorAllocation m_allocation; // Nuestro "ticket" del DescriptorAllocator
    };
}
