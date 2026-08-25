#pragma once

#include "D3DHelpers.h"
#include "DescriptorAllocator.h"
#include <wrl/client.h>
#include <string>

#include "UploadHeap.h"

namespace Bruno 
{
    enum class TextureFormat;
    class UploadContext;
    
    class Texture2D : public Asset
    {
        BR_RTTI_DECLARATION(Texture2D, Asset);
    
    public:
        // Carga una textura desde disco y reserva su lugar en el heap
        Texture2D(GraphicsDevice& device, UploadContext& uploadContext, DescriptorAllocator& srvAllocator, const std::wstring& filename);
        Texture2D(
            GraphicsDevice& device, 
            uint32_t width, 
            uint32_t height, 
            TextureFormat format, 
            DescriptorAllocator& srvAllocator, // Mega Heap global para Bindless
            DescriptorAllocator& rtvAllocator, // Nuevo Allocator exclusivo de RTVs
            const std::wstring& name = L"Render_Target"
        );
        Texture2D() = default;
        
        ~Texture2D() override = default;
        
        void AttachNativeResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);
        
        // Necesario para el Material (Lo usa la CPU para copiar el descriptor usando CopyDescriptorsSimple)
        //[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return m_srvAllocation.CPU; }
        
        [[nodiscard]] ID3D12Resource* GetResource() const { return m_resource.Get(); }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const { return m_rtvHandle; }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return m_srvAllocation.CPU; }
        
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
        
        std::unique_ptr<UploadHeap> m_uploadHeap; // Necesario para subir datos a VRAM
        DescriptorAllocation m_srvAllocation; // Nuestro "ticket" del DescriptorAllocator. Para el SRV (Lectura en Shader)
        DescriptorAllocation m_rtvAllocation; // NUEVO: Ticket del RTV (En el Heap de Render Targets)
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle{};
    };
}
