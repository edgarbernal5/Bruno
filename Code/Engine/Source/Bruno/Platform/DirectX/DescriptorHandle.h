#pragma once

#include "D3DHelpers.h"

namespace Bruno::DX {

    struct DescriptorHandle 
    {
        D3D12_CPU_DESCRIPTOR_HANDLE CPU{ 0 };
        D3D12_GPU_DESCRIPTOR_HANDLE GPU{ 0 };

        // C++17: Inicialización y validación limpia
        [[nodiscard]] bool IsValid() const { return CPU.ptr != 0; }
        [[nodiscard]] bool IsShaderVisible() const { return GPU.ptr != 0; }
    };

}