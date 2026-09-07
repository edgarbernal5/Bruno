#pragma once

#include "D3DHelpers.h"
#include "GpuBuffer.h"

#include <wrl/client.h>

namespace Bruno 
{
    class UploadHeap : public GpuBuffer
    {
    public:
        UploadHeap(GraphicsDevice& device, size_t sizeInBytes);
        ~UploadHeap() override;

        // Copia datos de CPU a este buffer
        void UploadData(const void* data, UINT64 size, UINT64 offset = 0);
        
    private:
    };
}