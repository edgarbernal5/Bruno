#pragma once

#include "D3DHelpers.h"
#include "DescriptorAllocator_Gem.h"
#include <wrl/client.h>
#include <string>

#include "UploadHeap.h"

namespace Bruno::DX 
{
    class UploadContext;
    
    class Texture2D : public Asset
    {
        BR_RTTI_DECLARATION(Texture2D, Asset);
    
    public:
        // Carga una textura desde disco y reserva su lugar en el heap
        Texture2D(DX::GraphicsDevice& device, DX::UploadContext& uploadContext, DescriptorAllocator& srvAllocator, const std::wstring& filename);
        ~Texture2D() = default;

        // Necesario para el Render Loop (Lo lee la GPU al dibujar)
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_allocation.GPU; }
        
        // Necesario para el Material (Lo usa la CPU para copiar el descriptor usando CopyDescriptorsSimple)
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return m_allocation.CPU; }
        
        [[nodiscard]] ID3D12Resource* GetResource() const { return m_textureResource.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_textureResource;
        std::unique_ptr<UploadHeap> m_uploadHeap; // Necesario para subir datos a VRAM
        DescriptorAllocation m_allocation; // Nuestro "ticket" del DescriptorAllocator
    };
}
