#pragma once

#include <cstdint>

namespace Bruno::Graphics::Core
{
    constexpr uint32_t FRAMES_IN_FLIGHT_COUNT{ 2 };
    constexpr uint32_t BACK_BUFFER_COUNT{ 3 };
    constexpr uint32_t MAX_BACK_BUFFER_COUNT{ 3 };
    constexpr uint32_t SHADER_PROGRAMS_COUNT{ 2 };
    constexpr uint32_t INVALID_RESOURCE_TABLE_INDEX{ UINT_MAX };

    constexpr uint8_t RESOURCE_SPACES_COUNT{ 4 };

    constexpr uint32_t MAX_QUEUED_BARRIERS{ 16 };
    constexpr uint32_t MAX_TEXTURE_SUBRESOURCE_COUNT{ 32 };

    constexpr uint32_t SAMPLER_DESCRIPTORS_COUNT{ 1 };

    constexpr uint32_t RTV_STAGING_DESCRIPTORS_COUNT{ 256 };
    constexpr uint32_t DSV_STAGING_DESCRIPTORS_COUNT{ 32 };
    constexpr uint32_t SRV_STAGING_DESCRIPTORS_COUNT{ 4096 };
    constexpr uint32_t RESERVED_SRV_DESCRIPTORS_COUNT{ 8192 };
    constexpr uint32_t SRV_RENDER_PASS_USER_DESCRIPTORS_COUNT{ 65536 };

    constexpr uint8_t PER_OBJECT_SPACE{ 0 };
}