#pragma once
#include "GraphicsDevice.h"
#include <wrl.h>
#include <cstdint>

namespace Bruno
{
    class DepthBuffer
    {
    public:
        DepthBuffer(GraphicsDevice& device, uint32_t width, uint32_t height);
        ~DepthBuffer() = default;

        // Si la ventana cambia de tamaño, el Depth Buffer también debe hacerlo
        void Resize(uint32_t width, uint32_t height);

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetView() const
        {
            return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        }

    private:
        void CreateResourceAndDescriptor();
        
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_depthTexture;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        uint32_t m_width;
        uint32_t m_height;
    };
}