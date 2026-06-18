#pragma once
#include "SurfaceTypes.h"

namespace Bruno::DX
{
	
	class SwapChain;
	class DepthBuffer;
	class GraphicsDevice;
	
    class Surface
    {
    public:
    	Surface(GraphicsDevice& device, const DX::SurfaceWindowParameters& parameters);
    	~Surface();
    	
    	[[nodiscard]] uint32_t GetCurrentBackBufferIndex() const;
    	[[nodiscard]] ID3D12Resource* GetCurrentBackBuffer() const;
    	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;
    	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDepthBufferView() const;

    	void Present(bool vsync = true);
    	void Resize(uint32_t width, uint32_t height);
    	
    private:
    	DX::GraphicsDevice& m_device;
    	
    	DX::SurfaceWindowParameters	m_parameters;
    	
    	std::unique_ptr<SwapChain> m_swapChain;
    	std::unique_ptr<DepthBuffer> m_depthBuffer;
	};
}
