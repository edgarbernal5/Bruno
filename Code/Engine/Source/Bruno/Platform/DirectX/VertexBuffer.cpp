#include "brpch.h"
#include "VertexBuffer.h"

#include "UploadContext.h"
#include "GraphicsDevice.h"

namespace Bruno
{
    VertexBuffer::VertexBuffer(GraphicsDevice& device, size_t vertexCount, size_t stride, const std::wstring& debugName)
        : GpuBuffer(device, vertexCount * stride, debugName), 
          m_vertexCount(static_cast<uint32_t>(vertexCount)), 
          m_stride(static_cast<uint32_t>(stride))
    {
        BR_ASSERT(vertexCount > 0 && stride > 0, "VertexBuffer dinámico inválido.");
        InitializeView();
    }

    VertexBuffer::VertexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const void* data, size_t vertexCount, size_t stride, const std::wstring& debugName) 
        : GpuBuffer(device, uploadContext, data, vertexCount * stride, debugName), 
          m_vertexCount(static_cast<uint32_t>(vertexCount)), 
          m_stride(static_cast<uint32_t>(stride))
    {
        BR_ASSERT(data != nullptr && vertexCount > 0 && stride > 0, "VertexBuffer estático inválido.");
        InitializeView();
    }

    void VertexBuffer::InitializeView()
    {
        m_view.BufferLocation = GetGPUAddress();
        m_view.SizeInBytes = static_cast<UINT>(m_size);
        m_view.StrideInBytes = m_stride;
    }
}
