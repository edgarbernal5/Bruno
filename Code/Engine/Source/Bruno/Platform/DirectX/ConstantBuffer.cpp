#include "brpch.h"
#include "ConstantBuffer.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    ConstantBuffer::ConstantBuffer(GraphicsDevice* device, size_t bufferSize)
    {
        auto nativeDevice = device->GetNativeDevice();
        
        // En DX12, los Constant Buffers DEBEN estar alineados a 256 bytes
        size_t alignedSize = (bufferSize + 255) & ~255;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD; // Memoria puente (CPU -> GPU)

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = alignedSize;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.SampleDesc.Count = 1;

        HRESULT hr = nativeDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer));

        if (FAILED(hr))
        {
            throw std::runtime_error("Fallo al crear el Constant Buffer.");
        }

        // Mapeamos el puntero de memoria de la GPU a la CPU permanentemente (Práctica recomendada en DX12)
        D3D12_RANGE readRange = { 0, 0 }; // No vamos a leer desde la CPU
        m_buffer->Map(0, &readRange, &m_mappedData);
    }

    ConstantBuffer::~ConstantBuffer()
    {
        if (m_buffer)
        {
            m_buffer->Unmap(0, nullptr);
        }
    }

    void ConstantBuffer::Update(const void* data, size_t dataSize)
    {
        // Copia directa de RAM (tu matriz C++) a la memoria accesible por la GPU
        ::memcpy(m_mappedData, data, dataSize);
    }
}
