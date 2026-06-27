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
        // Constructor Original (Estático - DEFAULT HEAP)
        IndexBuffer(GraphicsDevice& device, UploadContext& uploadContext, const std::vector<uint32_t>& indices);

        // NUEVO: Constructor Dinámico (UPLOAD HEAP)
        // No necesita UploadContext ni datos iniciales, solo el tamaño máximo en bytes.
        IndexBuffer(GraphicsDevice& device, size_t bufferSize, bool isDynamic = true);
        
        ~IndexBuffer() = default;
        
        // NUEVO: Método para actualizar los datos desde la CPU
        void Update(const void* data, size_t size);
        
        [[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& GetView() const { return m_view; }
        [[nodiscard]] uint32_t GetIndicesCount() const { return m_indicesCount; }

        [[nodiscard]] ID3D12Resource* GetBuffer() const { return m_buffer.Get(); }
        
        UINT GetSizeInBytes() const;
        
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;
        D3D12_INDEX_BUFFER_VIEW m_view;
        uint32_t m_indicesCount;
        bool m_isDynamic { false };
    };
}