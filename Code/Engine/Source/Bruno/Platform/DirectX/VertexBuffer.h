#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "GpuBuffer.h"

namespace Bruno
{
    class UploadContext;
    
    class VertexBuffer : public GpuBuffer
    {
    public:
        // Constructor Dinámico
        VertexBuffer(GraphicsDevice& device, size_t vertexCount, size_t stride);
        
        // Constructor Estático
        VertexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const void* data, size_t vertexCount, size_t stride);
        
        [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return m_view; }

        [[nodiscard]] uint32_t GetVertexCount() const { return m_vertexCount; }
        [[nodiscard]] uint32_t GetStride() const { return m_stride; }
        
        UINT GetSizeInBytes() const;
        
    private:
        void InitializeView();
        
        D3D12_VERTEX_BUFFER_VIEW m_view{};
        uint32_t m_vertexCount = 0;
        uint32_t m_stride = 0;
        bool m_isDynamic { false };
    };
}
