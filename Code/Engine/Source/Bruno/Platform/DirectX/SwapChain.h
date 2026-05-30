#pragma once

#include "D3DHelpers.h"

namespace Bruno::DX
{
    class GraphicsDevice; // Forward declaration (KISS)
    
    class SwapChain
    {
    public:
        static constexpr uint32_t FrameCount = 2; // Double Buffering

        SwapChain(GraphicsDevice& device, void* windowHandle, uint32_t width, uint32_t height);
        ~SwapChain() = default;

        void Present();
        void Resize(uint32_t width, uint32_t height);
        
        [[nodiscard]] uint32_t GetCurrentBackBufferIndex() const;
        [[nodiscard]] ID3D12Resource* GetCurrentBackBuffer() const;
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
    
    private:
        void CreateSwapChain(void* windowHandle, uint32_t width, uint32_t height);
        void CreateRenderTargets();

        GraphicsDevice& m_device; // Inyección de dependencias
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
        
        uint32_t m_rtvDescriptorSize{ 0 };
        uint32_t m_currentFrameIndex{ 0 };
    };

}