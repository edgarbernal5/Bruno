#pragma once

#include "D3DHelpers.h"

#include "SurfaceTypes.h"

namespace Bruno
{
    class GraphicsDevice; // Forward declaration (KISS)
    
    class SwapChain
    {
    public:
        // C++17: HWND es un void*, usamos dependencias claras
        SwapChain(GraphicsDevice& device, SurfaceWindowParameters const& parameters);
        ~SwapChain();

        void Present(bool vsync = true);
        void Resize(uint32_t width, uint32_t height);

        [[nodiscard]] uint32_t GetCurrentBackBufferIndex() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;
        
        // Descriptor Handle para usar en el pipeline
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;

    private:
        void UpdateRenderTargetViews();

    private:
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
        
        static const uint8_t BufferCount = 2; // Doble buffering
        uint32_t m_currentBufferIndex;

        // Recursos reales (Texturas 2D en VRAM)
        Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[BufferCount];
        
        // Montículo de descriptores para los Render Targets
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        uint32_t m_rtvDescriptorSize;
        
        SurfaceWindowParameters m_parameters;
    };

}