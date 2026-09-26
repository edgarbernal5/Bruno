#pragma once

namespace Bruno
{
    struct DescriptorAllocation;

    class TextureArrayResource
    {
    public:
        TextureArrayResource() = default;
        virtual ~TextureArrayResource();
        
        virtual const DescriptorAllocation& GetDSVHandle(uint32_t index) const = 0;
    };
}