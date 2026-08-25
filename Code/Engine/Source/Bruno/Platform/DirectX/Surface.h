#pragma once
#include "SurfaceTypes.h"

namespace Bruno
{
	class Texture2D;
	class SwapChain;
	class DepthBuffer;
	class GraphicsDevice;
	
    class Surface
    {
    public:
    	Surface(GraphicsDevice& device, const SurfaceWindowParameters& parameters);
    	~Surface();
    	
    	[[nodiscard]] uint32_t GetCurrentBackBufferIndex() const;
    	[[nodiscard]] Texture2D* GetCurrentRenderTarget() const;
    	[[nodiscard]] DepthBuffer* GetDepthBuffer() const;

    	void Present(bool vsync = true);
    	void Resize(uint32_t width, uint32_t height);
    	
    private:
    	GraphicsDevice& m_device;
    	
    	SurfaceWindowParameters	m_parameters;
    	
    	std::unique_ptr<SwapChain> m_swapChain;
    	std::unique_ptr<DepthBuffer> m_depthBuffer;
	};
}
