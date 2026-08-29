#pragma once

#include "D3DConstants.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    struct SurfaceWindowParameters
    {
        uint32_t Width;
        uint32_t Height;
        TextureFormat BackBufferFormat = TextureFormat::R8G8B8A8_Unorm;// DXGI_FORMAT_R8G8B8A8_UNORM;
        uint32_t BackBufferCount = Graphics::Core::BACK_BUFFER_COUNT;
        TextureFormat DepthBufferFormat = TextureFormat::D32_Float;
        HWND WindowHandle;
        bool Fullscreen = false;
    };
}
