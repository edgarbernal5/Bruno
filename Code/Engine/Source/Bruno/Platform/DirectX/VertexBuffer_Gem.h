#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace Bruno::DX
{
    class VertexBuffer {
    public:
        // C++17: Pasamos la lista de comandos para grabar la operación de copia al instante
        VertexBuffer(ID3D12Device* device, 
                     ID3D12GraphicsCommandList* cmdList, 
                     const void* data, 
                     size_t vertexCount, 
                     size_t vertexStride);
        
        ~VertexBuffer() = default;

        // Limpia el "puente" de memoria una vez que la GPU haya terminado de copiar
        void FreeUploadBuffer();

        [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return m_view; }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_defaultBuffer; // Memoria ultra rápida en VRAM
        Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;  // Puente temporal en RAM
        
        D3D12_VERTEX_BUFFER_VIEW m_view{};
    };
}