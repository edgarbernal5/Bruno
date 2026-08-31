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
        Texture2D() = default;
        
        // Carga desde disco inyectando el SRV directamente en el Mega Heap global
        Texture2D(GraphicsDevice& device, UploadContext& uploadContext, DescriptorAllocator& srvAllocator, const std::wstring& filename);
        
        // Constructor procedural con identidad dual para G-Buffers (RTV temporal + SRV Bindless)
        Texture2D(
            GraphicsDevice& device, 
            uint32_t width, 
            uint32_t height, 
            TextureFormat format, 
            DescriptorAllocator& srvAllocator, // Mega Heap global para Bindless
            DescriptorAllocator& rtvAllocator, // Nuevo Allocator exclusivo de RTVs
            const std::wstring& name = L"Render_Target"
        );
        ~Texture2D() override = default;
        
        void AttachNativeResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const DescriptorAllocation& rtvAllocation);
        
        [[nodiscard]] uint32_t GetBindlessIndex() const { return m_srvAllocation.Index; }
        
        [[nodiscard]] ID3D12Resource* GetResource() const { return m_resource.Get(); }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const { return m_rtvAllocation.CPU; }
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return m_srvAllocation.CPU; }
        \

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
        
        DescriptorAllocation m_srvAllocation; // Nuestro "ticket" del DescriptorAllocator. Para el SRV (Lectura en Shader)
        DescriptorAllocation m_rtvAllocation; // NUEVO: Ticket del RTV (En el Heap de Render Targets)
    };
}
