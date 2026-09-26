#pragma once

#include <wrl.h>
#include <cstdint>

#include "DescriptorAllocator.h"
#include "Texture2D.h"

namespace Bruno
{
    enum class TextureFormat;
    class GraphicsDevice;
    
    class DepthBuffer : public Texture2D
    {
        BR_RTTI_DECLARATION(DepthBuffer, Texture2D);
        
    public:
        DepthBuffer(GraphicsDevice& device, uint32_t width, uint32_t height, TextureFormat format);
        ~DepthBuffer() override;

        void Resize(uint32_t width, uint32_t height);

        // Devuelve un handle ligero asignado desde un pool global en la inicialización
        [[nodiscard]] const DescriptorAllocation& GetDSVAllocation() const { return m_dsvHandle; }
    
        // Si necesitas leerlo en un shader (SSAO, Shadows)
        //[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return m_srvHandle.CPU; }
        
    private:
        void CreateResourceAndDescriptor();
        
        GraphicsDevice& m_device;
       
        DescriptorAllocation m_dsvHandle;
        
    };
}
