#pragma once

#include "D3DHelpers.h"
#include "GraphicsResource.h"

#include <wrl/client.h>
#include <vector>

namespace Bruno 
{
    class UploadHeap : public GraphicsResource
    {
    public:
        // Crea un buffer de tamaño específico en la memoria de subida
        UploadHeap(GraphicsDevice& device, size_t sizeInBytes);
        ~UploadHeap() override;

        // Copia datos de CPU a este buffer
        void UploadData(const void* data, UINT64 size, UINT64 offset = 0);

        [[nodiscard]] UINT64 GetSize() const { return m_size; }
        [[nodiscard]] void* GetMappedData() const { return m_mappedData; }
    private:
        void* m_mappedData = nullptr; // Puntero directo a la memoria de la CPU
        UINT64 m_size = 0;
    };
}