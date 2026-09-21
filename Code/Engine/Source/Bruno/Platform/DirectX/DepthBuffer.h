#pragma once

#include <wrl.h>
#include <cstdint>

#include "DescriptorAllocator.h"
#include "GraphicsResource.h"

namespace Bruno
{
    enum class TextureFormat;
    class GraphicsDevice;
    
    class DepthBuffer : public GraphicsResource
    {
    public:
        DepthBuffer(GraphicsDevice& device, uint32_t width, uint32_t height, TextureFormat format);
        ~DepthBuffer();

        void Resize(uint32_t width, uint32_t height);

        // Devuelve un handle ligero asignado desde un pool global en la inicialización
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const { return m_dsvHandle.CPU; }
    
        // Si necesitas leerlo en un shader (SSAO, Shadows)
        //[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return m_srvHandle.CPU; }
        
    private:
        void CreateResourceAndDescriptor();
        
        GraphicsDevice& m_device;
       
        DescriptorAllocation m_srvHandle;
        DescriptorAllocation m_dsvHandle;
        
        uint32_t m_width;
        uint32_t m_height;
        TextureFormat m_format;
    };
}
