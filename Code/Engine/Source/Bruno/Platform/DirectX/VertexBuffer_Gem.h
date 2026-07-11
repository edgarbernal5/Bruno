#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace Bruno::DX
{
    class UploadContext;
    
    class VertexBuffer
    {
    public:
        // Constructor Original (Estático - DEFAULT HEAP)
        VertexBuffer(GraphicsDevice& device, DX::UploadContext& uploadContext, const void* data, size_t vertexCount, size_t vertexStride);
        
        // Constructor Dinámico (UPLOAD HEAP)
        // No necesita UploadContext ni datos iniciales, solo el tamaño máximo en bytes.
        explicit VertexBuffer(GraphicsDevice& device, size_t bufferSize, uint32_t stride, bool isDynamic = true);
        
        ~VertexBuffer() = default;

        // Método para actualizar los datos desde la CPU
        void Update(const void* data, size_t size);
        
        [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return m_view; }
        [[nodiscard]] ID3D12Resource* GetBuffer() const { return m_buffer.Get(); }

        UINT GetSizeInBytes() const;
        
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer; // Memoria ultra rápida en VRAM
        
        D3D12_VERTEX_BUFFER_VIEW m_view{};
        bool m_isDynamic { false };
    };
}
