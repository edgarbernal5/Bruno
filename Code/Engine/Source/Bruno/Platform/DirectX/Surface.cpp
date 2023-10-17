#include "brpch.h"
#include "Surface.h"

#include "GraphicsDevice.h"
#include "CommandQueue.h"
#include "Texture.h"

namespace Bruno
{
	Surface::Surface(const SurfaceWindowParameters& parameters) :
		m_parameters(parameters)
	{
	}

	Surface::~Surface()
	{
		auto device = Graphics::GetDevice();
		device->WaitForIdle();
		
		ReleaseBackBufferResources();
	}

	void Surface::Initialize()
	{
		GraphicsDevice* device = Graphics::GetDevice();

		const uint32_t backBufferWidth = std::max<uint32_t>(m_parameters.Width, 1u);
		const uint32_t backBufferHeight = std::max<uint32_t>(m_parameters.Height, 1u);
		const DXGI_FORMAT backBufferFormat = m_parameters.BackBufferFormat;
		m_parameters.BackBufferCount = std::min<uint32_t>(m_parameters.BackBufferCount, Graphics::Core::MAX_BACK_BUFFER_COUNT);

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
			device->GetGraphicsQueue()->GetQueue(),
			m_parameters.WindowHandle,
			&desc,
			&fullscreenDesc,
			nullptr,
			tempSwapChain.GetAddressOf()
		);
		ThrowIfFailed(hr);

		hr = tempSwapChain.As(&m_swapChain);

		// This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
		ThrowIfFailed(device->GetFactory()->MakeWindowAssociation(m_parameters.WindowHandle, DXGI_MWA_NO_ALT_ENTER));

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		for (uint32_t i = 0; i < m_parameters.BackBufferCount; ++i)
		{
			m_renderTargetData[i].RtvHandle = device->GetRtvDescriptionHeap().Allocate();
		}

		m_depthBuffer = std::make_unique<DepthBuffer>(backBufferWidth, backBufferHeight, m_parameters.DepthBufferFormat);
		Finalize();
	}

	void Surface::Present() const
	{
		ThrowIfFailed(m_swapChain->Present(0, 0)); //Present(1, 0));
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void Surface::Resize(uint32_t width, uint32_t height)
	{
		const uint32_t backBufferWidth = std::max<uint32_t>(width, 1u);
		const uint32_t backBufferHeight = std::max<uint32_t>(height, 1u);

		if (backBufferWidth == m_parameters.Width && backBufferHeight == m_parameters.Height)
			return;

		auto device = Graphics::GetDevice();
		device->WaitForIdle();

		ReleaseBackBufferResources();

		HRESULT hr = m_swapChain->ResizeBuffers(
			m_parameters.BackBufferCount,
			backBufferWidth,
			backBufferHeight,
			m_parameters.BackBufferFormat,
			0u
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
#ifdef BR_DEBUG
			char buffer[64] = {};
			sprintf_s(buffer, "Device Lost on ResizeBuffers: Reason code 0x%08X\n",
				static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? device->GetD3DDevice()->GetDeviceRemovedReason() : hr));
			OutputDebugStringA(buffer);

			BR_CORE_ERROR << buffer << std::endl;
#endif
			HandleDeviceLost();
			return;
		}
		else
		{
			ThrowIfFailed(hr);
		}
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_parameters.Width = backBufferWidth;
		m_parameters.Height = backBufferHeight;

		// set viewport and scissor rect
		m_viewport.TopLeftX = 0.f;
		m_viewport.TopLeftY = 0.f;
		m_viewport.Width = (float)backBufferWidth;
		m_viewport.Height = (float)backBufferHeight;
		m_viewport.MinDepth = D3D12_MIN_DEPTH;
		m_viewport.MaxDepth = D3D12_MAX_DEPTH;

		m_scissorRect = { 0, 0, (int32_t)backBufferWidth, (int32_t)backBufferHeight };

		m_depthBuffer.reset(new DepthBuffer(backBufferWidth, backBufferHeight, m_parameters.DepthBufferFormat));

		Finalize();
	}

	void Surface::HandleDeviceLost()
	{

	}

	void Surface::Finalize()
	{
		GraphicsDevice* device = Graphics::GetDevice();

		for (uint32_t i = 0; i < m_parameters.BackBufferCount; i++)
		{
			RenderTargetData& data{ m_renderTargetData[i] };

			ID3D12Resource* backBufferResource = nullptr;
			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferResource)));

			if (data.Resource == nullptr) {
				data.Resource = std::make_shared<Texture>();
			}
			data.Resource->m_rtvDescriptor = data.RtvHandle;
			data.Resource->m_resource = backBufferResource;
			data.Resource->m_state = D3D12_RESOURCE_STATE_PRESENT;
			data.Resource->m_desc = backBufferResource->GetDesc();

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			device->GetD3DDevice()->CreateRenderTargetView(data.Resource->GetResource(), &rtvDesc, data.RtvHandle.Cpu);
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

	void Surface::ReleaseBackBufferResources()
	{
		for (uint32_t i = 0; i < m_parameters.BackBufferCount; ++i)
		{
			SafeRelease(m_renderTargetData[i].Resource->m_resource);
		}
	}
}