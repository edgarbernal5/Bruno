#include "brpch.h"
#include "Surface.h"

#include "GraphicsDevice.h"
#include "CommandQueue.h"

namespace Bruno
{
	Surface::Surface(const SurfaceWindowParameters& parameters) :
		m_parameters(parameters)
	{
	}

	Surface::~Surface()
	{
		Release();
	}

	void Surface::Initialize()
	{
		GraphicsDevice* device = Graphics::GetDevice();

		const uint32_t backBufferWidth = std::max<uint32_t>(m_parameters.Width, 1u);
		const uint32_t backBufferHeight = std::max<uint32_t>(m_parameters.Height, 1u);
		const DXGI_FORMAT backBufferFormat = m_parameters.BackBufferFormat;

		uint32_t swapChainFlags = 0; //DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		//TODO: check tearing

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = m_parameters.BackBufferCount;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = swapChainFlags; //_allow_tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		desc.Format = backBufferFormat;
		desc.Height = m_parameters.Height;
		desc.Width = m_parameters.Width;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Stereo = false;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
		fullscreenDesc.Windowed = !m_parameters.Fullscreen;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
		HRESULT hr = device->GetFactory()->CreateSwapChainForHwnd(
			device->GetCommandQueue()->GetQueue(),
			m_parameters.WindowHandle,
			&desc,
			&fullscreenDesc,
			nullptr,
			tempSwapChain.GetAddressOf()
		);
		//ThrowIfFailed(hr);

		hr = tempSwapChain.As(&m_swapChain);

		// This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
		ThrowIfFailed(device->GetFactory()->MakeWindowAssociation(m_parameters.WindowHandle, DXGI_MWA_NO_ALT_ENTER));

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		for (uint32_t i = 0; i < Graphics::Core::FRAME_BUFFER_COUNT; ++i)
		{
			m_renderTargetData[i].Rtv = device->GetRtvDescriptionHeap().Allocate();
		}

		Finalize();
	}

	void Surface::Present() const
	{
		ThrowIfFailed(m_swapChain->Present(0, 0));
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void Surface::Resize(uint32_t width, uint32_t height)
	{
		const uint32_t backBufferWidth = std::max<uint32_t>(width, 1u);
		const uint32_t backBufferHeight = std::max<uint32_t>(height, 1u);

		if (backBufferWidth == m_parameters.Width && backBufferHeight == m_parameters.Height)
			return;

		m_parameters.Width = backBufferWidth;
		m_parameters.Height = backBufferHeight;

		Release();

		HRESULT hr = m_swapChain->ResizeBuffers(
			m_parameters.BackBufferCount,
			backBufferWidth,
			backBufferHeight,
			m_parameters.BackBufferFormat,
			0u
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
		}
		else {
			ThrowIfFailed(hr);
		}

		// set viewport and scissor rect
		m_viewport.TopLeftX = 0.f;
		m_viewport.TopLeftY = 0.f;
		m_viewport.Width = (float)width;
		m_viewport.Height = (float)height;
		m_viewport.MinDepth = D3D12_MIN_DEPTH;
		m_viewport.MaxDepth = D3D12_MAX_DEPTH;

		m_scissorRect = { 0, 0, (int32_t)width, (int32_t)height };

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		Finalize();
	}

	void Surface::CreateDependentResources()
	{
	}

	void Surface::Finalize()
	{
		GraphicsDevice* device = Graphics::GetDevice();

		// create RTVs for back-buffers
		for (uint32_t i = 0; i < Graphics::Core::FRAME_BUFFER_COUNT; i++)
		{
			RenderTargetData& data{ m_renderTargetData[i] };

			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&data.Resource)));
			
			device->GetD3DDevice()->CreateRenderTargetView(data.Resource, nullptr, data.Rtv.Cpu);
		}

		DXGI_SWAP_CHAIN_DESC desc{};
		ThrowIfFailed(m_swapChain->GetDesc(&desc));
		const uint32_t width{ desc.BufferDesc.Width };
		const uint32_t height{ desc.BufferDesc.Height };

		// set viewport and scissor rect
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = (float)width;
		m_viewport.Height = (float)height;
		m_viewport.MinDepth = D3D12_MIN_DEPTH;
		m_viewport.MaxDepth = D3D12_MAX_DEPTH;

		m_scissorRect = { 0, 0, (int32_t)width, (int32_t)height };
	}

	void Surface::Release()
	{
		for (uint32_t i = 0; i < m_parameters.BackBufferCount; ++i)
		{
			if (m_renderTargetData[i].Resource)
			{
				m_renderTargetData[i].Resource->Release();
				m_renderTargetData[i].Resource = nullptr;
			}
		}
	}
}