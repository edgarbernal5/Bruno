#pragma once

#include "D3DCommon.h"

namespace Bruno::Graphics::Core
{
    constexpr uint32_t FRAME_BUFFER_COUNT{ 3 };
    constexpr uint32_t UPLOAD_FRAME_COUNT{ 3 };
}

namespace Bruno::Device::Core
{
    void EnableDebugLayer();
}