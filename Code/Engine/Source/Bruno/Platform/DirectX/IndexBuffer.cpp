#include "brpch.h"
#include "IndexBuffer.h"

#include "UploadContext.h"

namespace Bruno
{
    IndexBuffer::IndexBuffer(GraphicsDevice& device, size_t indexCount, bool use16BitIndices, const std::wstring& debugName) : 
        GpuBuffer(device, indexCount * (use16BitIndices ? 2 : 4), debugName), 
        m_indicesCount(static_cast<uint32_t>(indexCount)), 
        m_format(use16BitIndices ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT) 
    {
        BR_ASSERT(indexCount > 0, "IndexBuffer dinámico no puede tener 0 índices.");
        InitializeView();
    }

    IndexBuffer::IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint32_t>& indices, const std::wstring& debugName) : 
        GpuBuffer(device, uploadContext, indices.data(), indices.size() * sizeof(uint32_t), debugName), 
        m_indicesCount(static_cast<uint32_t>(indices.size())), 
        m_format(DXGI_FORMAT_R32_UINT)
    {
        BR_ASSERT(!indices.empty(), "IndexBuffer estático (32-bit) no puede estar vacío.");
        InitializeView();
    }

    IndexBuffer::IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint16_t>& indices, const std::wstring& debugName) : 
        GpuBuffer(device, uploadContext, indices.data(), indices.size() * sizeof(uint32_t), debugName), 
        m_indicesCount(static_cast<uint32_t>(indices.size())), 
        m_format(DXGI_FORMAT_R16_UINT)
    {
        BR_ASSERT(!indices.empty(), "IndexBuffer estático (16-bit) no puede estar vacío.");
        InitializeView();
    }

    void IndexBuffer::InitializeView()
    {
        m_view.BufferLocation = GetGPUAddress();
        m_view.Format = m_format;
        m_view.SizeInBytes = static_cast<UINT>(m_size);
    }
}
