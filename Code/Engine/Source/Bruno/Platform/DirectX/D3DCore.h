#pragma once

#include "D3DCommon.h"

namespace Bruno::Graphics::Core
{
    constexpr uint32_t FRAMES_IN_FLIGHT_COUNT{ 2 };
    constexpr uint32_t BACK_BUFFER_COUNT{ 3 };
    constexpr uint32_t MAX_BACK_BUFFER_COUNT{ 3 };
    constexpr uint32_t UPLOAD_FRAME_COUNT{ 3 };
    constexpr uint32_t SHADER_PROGRAMS_COUNT{ 2 };
    constexpr uint32_t INVALID_RESOURCE_TABLE_INDEX{ 2 };

    constexpr uint8_t NUM_RESOURCE_SPACES{ 4 };

    constexpr uint32_t MAX_QUEUED_BARRIERS{ 16 };
    constexpr uint32_t MAX_TEXTURE_SUBRESOURCE_COUNT{ 32 };

    constexpr uint32_t NUM_SAMPLER_DESCRIPTORS = 6;

    constexpr uint32_t NUM_RTV_STAGING_DESCRIPTORS{ 256 };
    constexpr uint32_t NUM_DSV_STAGING_DESCRIPTORS{ 32 };
    constexpr uint32_t NUM_SRV_STAGING_DESCRIPTORS{ 4096 };
    constexpr uint32_t NUM_RESERVED_SRV_DESCRIPTORS{ 8192 };
    constexpr uint32_t NUM_SRV_RENDER_PASS_USER_DESCRIPTORS{ 65536 };
    
    constexpr uint8_t PER_OBJECT_SPACE = 0;

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