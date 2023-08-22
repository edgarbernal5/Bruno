#pragma once

#include "D3DCommon.h"

namespace Bruno::Graphics::Core
{
    constexpr uint32_t FRAME_BUFFER_COUNT{ 3 };
    constexpr uint32_t UPLOAD_FRAME_COUNT{ 3 };
    constexpr uint32_t SHADER_PROGRAMS_COUNT{ 2 };

    struct
    {
        const CD3DX12_HEAP_PROPERTIES DefaultHeap
        {
            D3D12_HEAP_TYPE_DEFAULT
        };

        const CD3DX12_HEAP_PROPERTIES UploadHeap
        {
            D3D12_HEAP_TYPE_UPLOAD
        };
    } HeapProperties;
}

namespace Bruno::Device::Core
{
    void EnableDebugLayer();
}