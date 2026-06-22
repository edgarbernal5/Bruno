#pragma once

#include "Device.h"
#include <wrl.h>
#include <vector>

namespace Bruno::DX
{
    class UploadContext;
    
    class IndexBuffer
    {
    public:
        IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint32_t>& indices);
        
        [[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetView() const { return m_view; }
        [[nodiscard]] uint32_t GetIndicesCount() const { return m_indicesCount; }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_defaultBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
        D3D12_INDEX_BUFFER_VIEW m_view;
        uint32_t m_indicesCount;
    };
}