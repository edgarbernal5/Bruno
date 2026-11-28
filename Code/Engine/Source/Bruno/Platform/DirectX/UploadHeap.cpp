#include "brpch.h"
#include "UploadHeap.h"

#include "GraphicsDevice.h"

namespace Bruno 
{
    UploadHeap::UploadHeap(GraphicsDevice& device, size_t sizeInBytes)
         // Llamamos al constructor dinámico de GpuBuffer.
         // Él se encarga del CreateCommittedResource, del UPLOAD_HEAP y del Map() persistente.
         : GpuBuffer(device, sizeInBytes, L"Upload_Heap_Buffer")
    {
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
        ::memcpy(static_cast<char*>(m_mappedData) + offset, data, size);
    }
}
