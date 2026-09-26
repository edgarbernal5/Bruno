#pragma once

#include "DescriptorAllocator.h"
#include "GraphicsResource.h"

namespace Bruno
{
    class TextureResource : public GraphicsResource
    {
        BR_RTTI_DECLARATION(TextureResource, GraphicsResource);
        
    public:
        TextureResource() = default;
        TextureResource(uint32_t width, uint32_t height, TextureFormat format, uint32_t mipMapLevels = 1);

        virtual ~TextureResource();
        
        [[nodiscard]] uint32_t GetBindlessIndex() const { return m_srvAllocation.Index; }
        
        [[nodiscard]] const DescriptorAllocation& GetSVRAllocation() const { return m_srvAllocation; }
        
        void SetSvrAllocation(const DescriptorAllocation& svrAllocation) { m_srvAllocation = svrAllocation; }
    protected:
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_mipMapLevels = 1;
        TextureFormat m_format;
        
        DescriptorAllocation m_srvAllocation;
    };
}
