#include "brpch.h"
#include "UploadHeap.h"

namespace Bruno 
{
    UploadHeap::UploadHeap(ID3D12Device* device, UINT64 size) : m_size(size)
    {
        // 1. Crear el heap de tipo UPLOAD
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        device->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)
        );

        // 2. Mapear la memoria (obtener puntero de acceso CPU)
        D3D12_RANGE readRange{ 0, 0 }; // No vamos a leer desde la CPU, solo escribir
        m_uploadBuffer->Map(0, &readRange, &m_mappedData);
    }

    void UploadHeap::UploadData(const void* data, UINT64 size, UINT64 offset)
    {
        BR_ASSERT(offset + size <= m_size, "¡Desbordamiento de buffer en UploadHeap!");
        
        // Copia directa de memoria (extremadamente rápido)
        memcpy(static_cast<char*>(m_mappedData) + offset, data, size);
    }
}