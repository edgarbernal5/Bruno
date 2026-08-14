#pragma once

#include "D3DHelpers.h"
#include "DescriptorAllocator.h"
#include <wrl/client.h>
#include <string>

#include "UploadHeap.h"

namespace Bruno 
{
    class UploadContext;
    
    class Texture2D : public Asset
    {
        BR_RTTI_DECLARATION(Texture2D, Asset);
    
    public:
        // Carga una textura desde disco y reserva su lugar en el heap
        Texture2D(GraphicsDevice& device, UploadContext& uploadContext, DescriptorAllocator& srvAllocator, const std::wstring& filename);
        Texture2D() = default;
        
        ~Texture2D() override = default;
        
        void AttachNativeResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);

        // Necesario para el Render Loop (Lo lee la GPU al dibujar)
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_srvAllocation.GPU; }
        
        // Necesario para el Material (Lo usa la CPU para copiar el descriptor usando CopyDescriptorsSimple)
        //[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return m_srvAllocation.CPU; }
        
        [[nodiscard]] ID3D12Resource* GetResource() const { return m_resource.Get(); }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const { return m_rtvHandle; }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return m_srvAllocation.CPU; }
        
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
        std::unique_ptr<UploadHeap> m_uploadHeap; // Necesario para subir datos a VRAM
        DescriptorAllocation m_srvAllocation; // Nuestro "ticket" del DescriptorAllocator. Para el SRV (Lectura en Shader)
        
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle{};
    };
}
