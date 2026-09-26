#pragma once

#include "Texture2D.h"

namespace Bruno
{
    class ColorBuffer : public Texture2D
    {
        BR_RTTI_DECLARATION(ColorBuffer, Texture2D);
        
    public:
        ColorBuffer() = default;
        ColorBuffer(
                GraphicsDevice& device, 
                uint32_t width, 
                uint32_t height, 
                TextureFormat format, 
                DescriptorAllocator& srvAllocator, // Mega Heap global para Bindless
                DescriptorAllocator& rtvAllocator, // Nuevo Allocator exclusivo de RTVs
                const std::wstring& name = L"Render_Target"
            );
        
        void AttachNativeResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const DescriptorAllocation& rtvAllocation);
        
        [[nodiscard]] const DescriptorAllocation& GetRTVAllocation() const { return m_rtvAllocation; }
        
    private:
        DescriptorAllocation m_rtvAllocation;
    };
}
