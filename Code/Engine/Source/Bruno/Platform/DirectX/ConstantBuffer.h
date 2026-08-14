#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace Bruno
{
    class GraphicsDevice;
    
    class ConstantBuffer
    {
    public:
        ConstantBuffer(GraphicsDevice* device, size_t bufferSize);
        ~ConstantBuffer();

        // Actualiza los datos desde C++ hacia la GPU (sin bloqueos)
        void Update(const void* data, size_t dataSize);

        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const
        { 
            return m_buffer->GetGPUVirtualAddress(); 
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;
        void* m_mappedData = nullptr;
    };
}
