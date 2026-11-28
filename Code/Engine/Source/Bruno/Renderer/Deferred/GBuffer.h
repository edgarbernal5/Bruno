#pragma once

#include <memory>

namespace Bruno
{
    class ColorBuffer;
    class GraphicsDevice;
    class Texture2D;
    class DepthBuffer;
    class DescriptorAllocator;

    class GBuffer
    {
    public:
        GBuffer() = default;
        ~GBuffer() = default;

        GBuffer(const GBuffer&) = delete;
        GBuffer& operator=(const GBuffer&) = delete;

        void Initialize(GraphicsDevice& device, DescriptorAllocator& srvHeap, DescriptorAllocator& rtvHeap, uint32_t width, uint32_t height);
        
        void Resize(GraphicsDevice& device, DescriptorAllocator& srvHeap, DescriptorAllocator& rtvHeap, uint32_t width, uint32_t height);

        ColorBuffer* GetAlbedoMetalness() const { return m_albedoMetalness.get(); }
        ColorBuffer* GetNormalRoughness() const { return m_normalRoughness.get(); }
        ColorBuffer* GetPosition() const { return m_position.get(); }
        DepthBuffer* GetDepth() const { return m_depthBuffer.get(); }

    private:
        std::unique_ptr<ColorBuffer> m_albedoMetalness; // SV_Target0
        std::unique_ptr<ColorBuffer> m_normalRoughness; // SV_Target1
        std::unique_ptr<ColorBuffer> m_position;        // SV_Target2
        
        std::unique_ptr<DepthBuffer> m_depthBuffer;   // Z-Buffer
    };

}