#pragma once

#include "DescriptorAllocator.h"

namespace Bruno
{
    
    class DynamicDescriptorAllocator : public DescriptorAllocator
    {
    public:
        DynamicDescriptorAllocator(GraphicsDevice& device, uint32_t capacity);
        
        void Reset();
        D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(GraphicsDevice& device, D3D12_CPU_DESCRIPTOR_HANDLE cpuStagingDescriptor);
    };
}
