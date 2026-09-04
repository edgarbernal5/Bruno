#include "brpch.h"
#include "VertexBuffer.h"

#include "UploadContext.h"
#include "GraphicsDevice.h"

namespace Bruno
{
    VertexBuffer::VertexBuffer(GraphicsDevice& device, size_t vertexCount, size_t stride)
        : GpuBuffer(device, vertexCount * stride, L"Dynamic_VB"), 
          m_vertexCount(static_cast<uint32_t>(vertexCount)), 
          m_stride(static_cast<uint32_t>(stride))
    {
        InitializeView();
    }

    VertexBuffer::VertexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const void* data, size_t vertexCount, size_t stride) 
        : GpuBuffer(device, uploadContext, data, vertexCount * stride, L"Static_VB"), 
          m_vertexCount(static_cast<uint32_t>(vertexCount)), 
          m_stride(static_cast<uint32_t>(stride))
    {
        InitializeView();
    }

    UINT VertexBuffer::GetSizeInBytes() const
    {
        return m_view.SizeInBytes;
    }

    void VertexBuffer::InitializeView()
    {
        m_view.BufferLocation = GetGPUAddress();
        m_view.SizeInBytes = static_cast<UINT>(m_size);
        m_view.StrideInBytes = m_stride;
    }
}
