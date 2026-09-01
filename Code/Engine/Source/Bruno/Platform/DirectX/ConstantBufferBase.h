#pragma once

#include <d3d12.h>

namespace Bruno
{
    class ConstantBufferBase
    {
    public:
        ConstantBufferBase() = default;
        ~ConstantBufferBase() = default;

        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const
        { 
            return m_gpuAddress; 
        }

    protected:
        D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress = 0;
    };
}