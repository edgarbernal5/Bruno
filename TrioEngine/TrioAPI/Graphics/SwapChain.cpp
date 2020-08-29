#include "stdafx.h"
#include "SwapChain.h"

namespace TrioEngine
{
	SwapChain::SwapChain(GraphicsDevice* graphicsDevice, int width, int height, HWND window, SurfaceFormat backBufferformat, uint32_t bufferCount)
		: m_graphicsDevice(graphicsDevice)
	{
		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = ToFormat(backBufferformat);
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = bufferCount;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0u;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		DX::ThrowIfFailed(
			graphicsDevice->GetDXGIFactory()->CreateSwapChainForHwnd
			(
				graphicsDevice->GetD3DDevice(),
				window,
				&swapChainDesc,
				&fsSwapChainDesc,
				nullptr,
				m_swapChain.ReleaseAndGetAddressOf()
		));

		// This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
		DX::ThrowIfFailed(graphicsDevice->GetDXGIFactory()->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER));
	}

	DeviceErrorStatus SwapChain::ResizeBuffers(uint32_t bufferCount, int width, int height, SurfaceFormat format, SwapChainFlags flags)
	{
		return (DeviceErrorStatus)m_swapChain->ResizeBuffers(
			bufferCount,
			width,
			height,
			ToFormat(format),
			0
		);
	}

	DeviceErrorStatus SwapChain::Present(uint32_t syncInterval, uint32_t flags)
	{
		return (DeviceErrorStatus)m_swapChain->Present(syncInterval, flags);
	}
}