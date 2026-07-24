#pragma once

#include "CommandContext.h"
#include "d3dx12.h" // Indispensable para matemáticas de DX12

namespace Bruno
{
    class UploadContext : public CommandContext
    {
    public:
        UploadContext(GraphicsDevice& device);
        
        // --- MÉTODOS DE SUBIDA AAA ---
        
        // Subir un buffer genérico (Vertex, Index, etc)
        void UploadBuffer(ID3D12Resource* destBuffer, const void* data, size_t size);
        
        // Subir una textura 2D manejando automáticamente los 256-bytes de alineación
        void UploadTexture(ID3D12Resource* destTexture, const D3D12_SUBRESOURCE_DATA& subResourceData);
        
        // ¡Se llama cuando la GPU ya avisó (Fence) que terminó de leer todo!
        void ClearGarbage();
        
    private:
        // Helper interno para crear memoria Upload visible a la CPU
        Microsoft::WRL::ComPtr<ID3D12Resource> CreateIntermediateBuffer(UINT64 sizeInBytes);

        // Aquí guardamos los búferes hasta que sea seguro borrarlos
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_intermediateBuffers;
    };
}
