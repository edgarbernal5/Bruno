#include "brpch.h"
#include "UploadHeap.h"

#include "GraphicsDevice.h"

namespace Bruno 
{
    UploadHeap::UploadHeap(GraphicsDevice& device, size_t sizeInBytes)
    {
        m_name = L"Upload_Heap_Buffer";
        m_currentState = ResourceState::GenericRead; // Estado nativo inmutable de Upload Heaps

        auto nativeDevice = device.GetNativeDevice();
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);

        HRESULT hr = nativeDevice->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(&m_resource)
        );

        if (FAILED(hr)) throw std::runtime_error("Fallo al crear UploadHeap");

        // Mapeo persistente O(1)
        CD3DX12_RANGE readRange(0, 0);
        m_resource->Map(0, &readRange, &m_mappedData);
    }

    UploadHeap::~UploadHeap()
    {
        if (m_resource)
        {
            m_resource->Unmap(0, nullptr);
        }
    }

    void UploadHeap::UploadData(const void* data, UINT64 size, UINT64 offset)
    {
        BR_ASSERT(offset + size <= m_size, "¡Desbordamiento de buffer en UploadHeap!");
        
        // Copia directa de memoria (extremadamente rápido)
        memcpy(static_cast<char*>(m_mappedData) + offset, data, size);
    }
}
