#pragma once

#include "D3DConstants.h"

namespace Bruno
{
    struct SurfaceWindowParameters
    {
        uint32_t Width;
        uint32_t Height;
        DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        uint32_t BackBufferCount = Graphics::Core::BACK_BUFFER_COUNT;
        DXGI_FORMAT DepthBufferFormat = DXGI_FORMAT_D32_FLOAT;
        HWND WindowHandle;
        bool Fullscreen = false;
    };
}
