#pragma once

#include <wrl.h>
#include <cstdint>

namespace Bruno
{
    enum class TextureFormat;
    class GraphicsDevice;
    
    class DepthBuffer
    {
    public:
        DepthBuffer(GraphicsDevice& device, uint32_t width, uint32_t height, TextureFormat format);
        ~DepthBuffer() = default;

        void Resize(uint32_t width, uint32_t height);

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetView() const
        {
            return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        }
        
        [[nodiscard]] ID3D12Resource* GetResource() const { return m_depthTexture.Get(); }

    private:
        void CreateResourceAndDescriptor();
        
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_depthTexture;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        uint32_t m_width;
        uint32_t m_height;
        TextureFormat m_format;
    };
}