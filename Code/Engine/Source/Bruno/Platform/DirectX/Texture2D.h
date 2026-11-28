#pragma once

#include "D3DHelpers.h"
#include "DescriptorAllocator.h"
#include <wrl/client.h>
#include <string>

#include "TextureResource.h"
#include "UploadHeap.h"

namespace Bruno 
{
    enum class TextureFormat;
    class UploadContext;
    
    class Texture2D : public TextureResource
    {
        BR_RTTI_DECLARATION(Texture2D, TextureResource);
    
    public:
        Texture2D() = default;
        
        // Carga desde disco inyectando el SRV directamente en el Mega Heap global
        Texture2D(GraphicsDevice& device, UploadContext& uploadContext, DescriptorAllocator& srvAllocator, const std::wstring& filename);
        ~Texture2D() override = default;
    
    private:
    };
}
