#pragma once

#include "D3DHelpers.h"
#include "DescriptorAllocator.h"

#include "SurfaceTypes.h"

namespace Bruno
{
    class Texture2D;
    class GraphicsDevice;
    
    class SwapChain
    {
    public:
        SwapChain(GraphicsDevice& device, SurfaceWindowParameters const& parameters);
        ~SwapChain();

        void Present(bool vsync = true);
        void Resize(uint32_t width, uint32_t height);

        [[nodiscard]] uint32_t GetCurrentBackBufferIndex() const;
        
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
        
        // DescriptorAllocator para los Render Targets
        std::unique_ptr<DescriptorAllocator> m_rtvAllocator;
        SurfaceWindowParameters m_parameters;
    };

}