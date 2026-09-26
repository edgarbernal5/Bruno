#pragma once
#include <memory>

#include "d3dx12.h"
#include "Bruno/Platform/DirectX/DescriptorAllocator.h"
#include "Bruno/Platform/DirectX/TextureArrayResource.h"
#include "Bruno/Platform/DirectX/TextureResource.h"
#include "Bruno/Scene/Constants.h"

namespace Bruno
{
    class DescriptorAllocator;
    class GraphicsDevice;
    
    class ShadowMapArray : public TextureResource, public TextureArrayResource
    {
        BR_RTTI_DECLARATION(ShadowMapArray, TextureResource);
        
    public:
        ShadowMapArray() = default;
        ~ShadowMapArray() override = default;
        
        void Initialize(GraphicsDevice* device, DescriptorAllocator& srvHeap, DescriptorAllocator& dsvHeap, uint32_t resolution, uint32_t numCascades = NUM_CASCADES);
    
        const DescriptorAllocation& GetDSVHandle(uint32_t index) const override { return m_dsvAllocations[index]; }
        
    private:
        std::vector<DescriptorAllocation> m_dsvAllocations;
        
        uint32_t m_resolution;
        uint32_t m_numCascades;
    };
}
