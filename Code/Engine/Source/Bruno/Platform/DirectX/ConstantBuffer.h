#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace Bruno
{
    class ConstantBuffer
    {
    public:
        // C++17: Usamos size_t para la alineación
        ConstantBuffer(ID3D12Device* device, size_t bufferSize);
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
