#include "brpch.h"
#include "Surface.h"

#include "GraphicsDevice.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DepthBuffer.h"

namespace Bruno
{
    Surface::Surface(GraphicsDevice& device, const SurfaceWindowParameters& parameters) :
        m_device(device),
        m_parameters(parameters)
    {
        m_swapChain = std::make_unique<SwapChain>(device, parameters);
        m_depthBuffer = std::make_unique<DepthBuffer>(device, parameters.Width, parameters.Height, parameters.DepthBufferFormat);
    }

    Surface::~Surface()
    {
        m_device.GetDirectCommandQueue().Flush();
    }

    uint32_t Surface::GetCurrentBackBufferIndex() const
    {
        return m_swapChain->GetCurrentBackBufferIndex();
    }

    Texture2D* Surface::GetCurrentRenderTarget() const
    {
        return m_swapChain->GetCurrentRenderTarget();
    }

    DepthBuffer* Surface::GetDepthBuffer() const
    {
        return m_depthBuffer.get();
    }

    void Surface::Present(bool vsync)
    {
        m_swapChain->Present(vsync);
    }

    void Surface::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            return;
        }
        
        m_device.GetDirectCommandQueue().Flush();
        
        m_swapChain->Resize(width, height);
        m_depthBuffer->Resize(width, height);
    }
}
