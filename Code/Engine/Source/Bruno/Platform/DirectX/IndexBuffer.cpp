#include "brpch.h"
#include "IndexBuffer.h"

#include "UploadContext.h"

namespace Bruno
{
    IndexBuffer::IndexBuffer(GraphicsDevice& device, size_t indexCount, bool use16BitIndices) : 
        GpuBuffer(device, indexCount * (use16BitIndices ? 2 : 4), L"Dynamic_IB"), 
        m_indicesCount(static_cast<uint32_t>(indexCount)), 
        m_format(use16BitIndices ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT) 
    {
        InitializeView();
    }

    IndexBuffer::IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint32_t>& indices) : 
        GpuBuffer(device, uploadContext, indices.data(), indices.size() * sizeof(uint32_t), L"Static_IB"), 
        m_indicesCount(static_cast<uint32_t>(indices.size())), 
        m_format(DXGI_FORMAT_R32_UINT)
    {
        InitializeView();
    }
    
    UINT IndexBuffer::GetSizeInBytes() const
    {
        return m_view.SizeInBytes;
    }

    void IndexBuffer::InitializeView()
    {
        // Preparar la vista (El "Ticket" para el RenderLoop)[cite: 1]
        m_view.BufferLocation = GetGPUAddress();
        m_view.Format = m_format;
        m_view.SizeInBytes = static_cast<UINT>(m_size);
    }
}
