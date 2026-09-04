#pragma once

#include "GraphicsDevice.h"
#include <wrl.h>
#include <vector>

#include "GpuBuffer.h"

namespace Bruno
{
    class UploadContext;
    
    class IndexBuffer : public GpuBuffer
    {
    public:
        // Constructor Dinámico (Ej: Para PrimitiveBatch)
        IndexBuffer(GraphicsDevice& device, size_t indexCount, bool use16BitIndices = false);
        
        // Constructor Estático (Ej: Para Modelos Importados)
        IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint32_t>& indices);
        
        [[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetView() const { return m_view; }
        [[nodiscard]] uint32_t GetIndicesCount() const { return m_indicesCount; }
        
        UINT GetSizeInBytes() const;
        
    private:
        void InitializeView();
        
        D3D12_INDEX_BUFFER_VIEW m_view{};
        uint32_t m_indicesCount = 0;
        DXGI_FORMAT m_format;
    };
}
