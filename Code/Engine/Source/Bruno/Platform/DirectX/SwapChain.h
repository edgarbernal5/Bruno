#pragma once

#include "D3DHelpers.h"
#include "DescriptorAllocator.h"

#include "SurfaceTypes.h"

namespace Bruno
{
    class Texture2D;
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
        
        Texture2D* GetCurrentRenderTarget() const;

    private:
        struct RenderTargetData
        {
            std::shared_ptr<Texture2D> Resource{ nullptr };
            DescriptorAllocation RtvHandle{};
        };
        
        void UpdateRenderTargetViews();
        
        GraphicsDevice& m_device;
        Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
        
        uint32_t m_currentBufferIndex;

        // Recursos reales (Texturas 2D en VRAM)
        RenderTargetData m_renderTargets[Graphics::Core::BACK_BUFFER_COUNT];
        
        // Montículo de descriptores para los Render Targets
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        uint32_t m_rtvDescriptorSize;
        
        SurfaceWindowParameters m_parameters;
    };

}