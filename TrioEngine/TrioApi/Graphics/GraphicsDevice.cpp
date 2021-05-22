#include "stdafx.h"
#include "GraphicsDevice.h"

#include "BlendState.h"
#include "ConstantBuffer.h"
#include "ConstantBufferCollection.h"
#include "DepthStencilBuffer.h"
#include "DepthStencilState.h"
#include "GraphicsAdapter.h"
#include "IndexBuffer.h"
#include "InputLayoutCache.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "SamplerStateCollection.h"
#include "Shader.h"
#include "TextureCollection.h"
#include "VertexBuffer.h"
#include "SwapChain.h"
#include "RenderTarget2D.h"

namespace TrioEngine
{
#if defined(_DEBUG) && defined(TRIO_DIRECTX)
	// Check for SDK Layer support.
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
			nullptr,                    // Any feature level will do.
			0,
			D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			nullptr,                    // No need to keep the D3D device reference.
			nullptr,                    // No need to know the feature level.
			nullptr                     // No need to keep the D3D device context reference.
		);

		return SUCCEEDED(hr);
	}
#endif

	GraphicsDevice::GraphicsDevice(GraphicsAdapter* adapter, PresentationParameters parameters) :
#if TRIO_DIRECTX
		m_currentD3dDepthStencilView(nullptr),
		m_defaultD3dDepthStencilView(nullptr),
		m_d3dMinFeatureLevel(D3D_FEATURE_LEVEL_10_0),
		m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
#endif

		m_currentDepthStencilBuffer(nullptr),

		m_vertexTextureCollection(nullptr),

		m_textureCollection(nullptr),
		m_samplerCollection(nullptr),
		m_adapter(adapter),
		m_presentationParameters(parameters),
		m_backBufferCount(2),
		m_vertexShader(nullptr),
		m_vertexShaderDirty(false),

		m_vertexConstantBuffers(nullptr),
		m_pixelConstantBuffers(nullptr),

		m_pixelShader(nullptr),
		m_pixelShaderDirty(false),

		m_rasterizerStateDirty(false),
		m_rasterizerState(nullptr),

		m_blendStateDirty(false),
		m_blendState(nullptr),

		m_depthStencilStateDirty(false),
		m_depthStencilState(nullptr)

	{
		CreateDeviceResources();
		CreateWindowSizeDependentResources();

		DepthStencilState::InitStates();
		RasterizerState::InitStates();
		BlendState::InitStates();
		SamplerState::InitStates();

		m_vertexTextureCollection = new TextureCollection(ShaderStage::Vertex);

		m_textureCollection = new TextureCollection(ShaderStage::Pixel);
		m_samplerCollection = new SamplerStateCollection(ShaderStage::Pixel);

		m_vertexConstantBuffers = new ConstantBufferCollection(ShaderStage::Vertex);
		m_pixelConstantBuffers = new ConstantBufferCollection(ShaderStage::Pixel);

		m_indexBufferDirty = true;
		m_vertexBufferDirty = true;

#ifdef TRIO_DIRECTX
		for (size_t i = 0; i < MaxVertexBuffers; i++)
		{
			m_vertexBuffers[i] = nullptr;
			m_vertexOffsets[i] = 0;
			m_vertexStrides[i] = 0;
		}
#endif
	}

	GraphicsDevice::~GraphicsDevice()
	{
	}

	void GraphicsDevice::ApplyState(bool applyShaders)
	{
		if (m_blendStateDirty)
		{
			m_blendState->ApplyState(this);
			m_blendStateDirty = false;
		}
		if (m_depthStencilStateDirty)
		{
			m_depthStencilState->ApplyState(this);
			m_depthStencilStateDirty = false;
		}
		if (m_rasterizerStateDirty)
		{
			m_rasterizerState->ApplyState(this);
			m_rasterizerStateDirty = false;
		}

		if (!applyShaders)
		{
			return;
		}

		if (m_indexBufferDirty)
		{
			if (m_indexBuffer)
			{
#ifdef TRIO_DIRECTX
				m_d3dContext->IASetIndexBuffer(m_indexBuffer->m_buffer, FormatHelper::ToFormat(m_indexBuffer->m_elementSize), 0);
#elif TRIO_OPENGL
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer->m_Buffer);
				GraphicsExtensions::checkGLError("Apply State GL_ELEMENT_ARRAY_BUFFER");
#endif
			}
			else
			{
#ifdef TRIO_DIRECTX
				m_d3dContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
#elif TRIO_OPENGL
				//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
			}
			m_indexBufferDirty = false;
		}

		if (m_vertexBufferDirty)
		{
			if (m_vertexBindings.size() > 0)
			{
#ifdef TRIO_DIRECTX
				m_d3dContext->IASetVertexBuffers(0, m_vertexBindings.size(), &m_vertexBuffers[0], m_vertexStrides, m_vertexOffsets);
#endif
			}
			else
			{
#ifdef TRIO_DIRECTX
				m_d3dContext->IASetVertexBuffers(0, 0, nullptr, 0, 0);
#endif
			}
		}

		if (m_vertexShaderDirty)
		{
#ifdef TRIO_DIRECTX
			if (m_vertexShader != nullptr)
			{
				m_d3dContext->VSSetShader(m_vertexShader->m_shaderPtr.m_vertexShader, nullptr, 0);
			}
			else
			{
				m_d3dContext->VSSetShader(nullptr, nullptr, 0);
			}
#endif
		}

		if (m_vertexShaderDirty || m_vertexBufferDirty)
		{
#ifdef TRIO_DIRECTX
			ID3D11InputLayout* layout = m_vertexShader->GetInputLayouts()->Get(m_vertexBindings[0].Buffer->m_vertexDeclaration);
			m_d3dContext->IASetInputLayout(layout);
#elif TRIO_OPENGL

#endif
			m_vertexShaderDirty = false;
			m_vertexBufferDirty = false;
		}

		if (m_pixelShaderDirty)
		{
#ifdef TRIO_DIRECTX
			if (m_pixelShader)
			{
				m_d3dContext->PSSetShader(m_pixelShader->m_shaderPtr.m_pixelShader, nullptr, 0);
			}
			else
			{
				m_d3dContext->PSSetShader(nullptr, nullptr, 0);
			}
#endif
			m_pixelShaderDirty = false;
		}

#ifdef TRIO_DIRECTX
		m_vertexConstantBuffers->SetConstantBuffers(this);
		m_pixelConstantBuffers->SetConstantBuffers(this);
#endif
	}

	bool GraphicsDevice::AreSameVertexBindings(VertexBufferBindings &bindings)
	{
		if (bindings.size() != m_vertexBindings.size())
			return false;

		for (int i = 0; i < bindings.size(); i++)
		{
			if (bindings[i].Buffer != m_vertexBindings[i].Buffer ||
				bindings[i].Offset != m_vertexBindings[i].Offset ||
				bindings[i].Stride != m_vertexBindings[i].Stride)
			{
				return false;
			}
		}
		return true;
	}

	void GraphicsDevice::Clear(ColorRGBA8 &color)
	{
		ClearOptions options = ClearOptions::Target;

		if (m_currentD3dDepthStencilView != nullptr)
		{
			options = options | ClearOptions::DepthBuffer;
			/*if (FormatHelper::HasStencil(m_depthStencilBuffer->GetDepthFormat()))
				options = options | ClearOptions::Stencil;*/
		}

		Clear(options, color, m_viewport.maxDepth, 0);
	}

	void GraphicsDevice::Clear(const float* color)
	{
		ClearOptions options = ClearOptions::Target;

		if (m_currentD3dDepthStencilView != nullptr)
		{
			options = options | ClearOptions::DepthBuffer;
			/*if (FormatHelper::HasStencil(m_depthStencilBuffer->GetDepthFormat()))
				options = options | ClearOptions::Stencil;*/
		}

		Clear(options, color, m_viewport.maxDepth, 0);
	}

	void GraphicsDevice::Clear(ClearOptions options, const float* color, float depth, uint8_t stencil)
	{
#ifdef TRIO_DIRECTX
		if ((options & ClearOptions::Target) == ClearOptions::Target)
		{
			for (int i = 0; i < m_currentD3dRenderTargets.size() && m_currentD3dRenderTargets[i] != nullptr; i++)
			//for (int i = 0; i < m_renderTargetBindings.size(); i++)
			{
				m_d3dContext->ClearRenderTargetView(m_currentD3dRenderTargets[i], color);
			}
		}
		uint32_t flags = 0;

		if ((options & ClearOptions::DepthBuffer) == ClearOptions::DepthBuffer)
			flags |= D3D11_CLEAR_DEPTH;

		if ((options & ClearOptions::Stencil) == ClearOptions::Stencil)
			flags |= D3D11_CLEAR_STENCIL;

		if (flags != 0 && m_currentD3dDepthStencilView != nullptr)
		{
			m_d3dContext->ClearDepthStencilView(m_currentD3dDepthStencilView, flags, depth, stencil);
		}

		//TODO: mover esto de aca?
		m_d3dContext->OMSetRenderTargets(1, m_currentD3dRenderTargets.data(), m_currentD3dDepthStencilView);

		//m_d3dContext->OMSetRenderTargets(m_renderTargetBindings.size(), &m_currentD3dRenderTargets[0], m_currentD3dDepthStencilView);

		m_d3dContext->RSSetViewports(1, m_viewport.Get11());
#endif
	}

	void GraphicsDevice::CreateDeviceResources()
	{
#ifdef TRIO_DIRECTX
		// D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		//0
		uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
		if (SdkLayersAvailable())
		{
			// If the project is in a debug build, enable debugging via SDK Layers with this flag.
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}
		else
		{
			OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
		}
#endif
		CreateFactory();

		// Determines whether tearing support is available for fullscreen borderless windows.
		//TODO: AllowTearing

		// Disable HDR if we are on an OS that can't support FLIP swap effects
		//TODO:
		// Determine DirectX hardware feature levels this app will support.
		static const D3D_FEATURE_LEVEL s_featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		uint32_t featLevelCount = 0;
		for (; featLevelCount < _countof(s_featureLevels); ++featLevelCount)
		{
			if (s_featureLevels[featLevelCount] < m_d3dMinFeatureLevel)
				break;
		}

		IDXGIAdapter1* adapter = m_adapter->GetD3DAdapter();

		D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		// Create the Direct3D 11 API device object and a corresponding context.
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

		HRESULT hr = E_FAIL;
		if (adapter)
		{
			hr = D3D11CreateDevice(
				adapter,
				D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				creationFlags,
				s_featureLevels,
				ARRAYSIZE(s_featureLevels),
				D3D11_SDK_VERSION,
				device.GetAddressOf(),  // Returns the Direct3D device created.
				&m_d3dFeatureLevel,     // Returns feature level of device created.
				context.GetAddressOf()  // Returns the device immediate context.
			);

		}
#if defined(NDEBUG)
		else
		{
			throw std::exception("No Direct3D hardware device found");
		}
#else
		if (FAILED(hr))
		{
			// If the initialization fails, fall back to the WARP device.
			// For more information on WARP, see: 
			// http://go.microsoft.com/fwlink/?LinkId=286690
			hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
				nullptr,
				creationFlags,
				s_featureLevels,
				ARRAYSIZE(s_featureLevels),
				D3D11_SDK_VERSION,
				device.GetAddressOf(),
				&m_d3dFeatureLevel,
				context.GetAddressOf()
			);

			if (SUCCEEDED(hr))
			{
				OutputDebugStringA("Direct3D Adapter - WARP\n");
			}
		}
#endif

		DX::ThrowIfFailed(hr);

#ifndef NDEBUG
		Microsoft::WRL::ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(device.As(&d3dDebug)))
		{
			Microsoft::WRL::ComPtr<ID3D11InfoQueue> d3dInfoQueue;
			if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
			{
#ifdef _DEBUG
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
				D3D11_MESSAGE_ID hide[] =
				{
					D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				};
				D3D11_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = _countof(hide);
				filter.DenyList.pIDList = hide;
				d3dInfoQueue->AddStorageFilterEntries(&filter);
			}
		}
#endif

		// Obtain Direct3D 11.1 interfaces (if available)

		DX::ThrowIfFailed(device.As(&m_d3dDevice));
		DX::ThrowIfFailed(context.As(&m_d3dContext));
		DX::ThrowIfFailed(context.As(&m_d3dAnnotation));
#endif
	}

#if TRIO_DIRECTX
	void GraphicsDevice::CreateFactory()
	{
#if defined(_DEBUG) && (_WIN32_WINNT >= 0x0603 /*_WIN32_WINNT_WINBLUE*/)
		bool debugDXGI = false;
		{
			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				debugDXGI = true;

				ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

				DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
				{
					80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
				};
				DXGI_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = _countof(hide);
				filter.DenyList.pIDList = hide;
				dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
			}
		}

		if (!debugDXGI)
#endif

		DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
	}
#endif

	void GraphicsDevice::CreateWindowSizeDependentResources()
	{
#ifdef TRIO_DIRECTX
		// Clear the previous window size specific context.
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
		m_currentD3dRenderTargets = std::vector<ID3D11RenderTargetView*>(4, nullptr);

		m_defaultD3dRenderTarget.Reset();
		m_d3dDefaultRenderTargetView.Reset();

		m_defaultD3dDepthStencilView = nullptr;
		m_depthStencilBuffer.reset();
		m_d3dContext->Flush();

		// Determine the render target size in pixels.
		uint32_t backBufferWidth = std::max<uint32_t>(m_presentationParameters.GetBackBufferWidth(), 1);
		uint32_t backBufferHeight = std::max<uint32_t>(m_presentationParameters.GetBackBufferHeight(), 1);

		if (m_presentationParameters.GetHostHWND() == nullptr)
		{
			return;
		}

		if (m_swapChain)
		{
			// If the swap chain already exists, resize it.
			DeviceErrorStatus status = m_swapChain->ResizeBuffers
			(
				m_backBufferCount,
				backBufferWidth,
				backBufferHeight,
				m_presentationParameters.GetBackBufferFormat(),
				SwapChainFlags::None
			);

			if (status == DeviceErrorStatus::DeviceRemoved || status == DeviceErrorStatus::DeviceReset)
			{
#ifdef _DEBUG
				char buff[64] = {};
				sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n", (status == DeviceErrorStatus::DeviceRemoved) ? m_d3dDevice->GetDeviceRemovedReason() : (long)status);
				OutputDebugStringA(buff);
#endif
				// If the device was removed for any reason, a new device and swap chain will need to be created.
				HandleDeviceLost();

				// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
				// and correctly set up the new device.
				return;
			}
			else
			{
				DX::ThrowIfFailed(status);
			}
		}
		else
		{
			m_swapChain.reset(new SwapChain(this, backBufferWidth, backBufferHeight,
				m_presentationParameters.GetHostHWND(), m_presentationParameters.GetBackBufferFormat(),
				m_backBufferCount));
		}

		// Handle color space settings for HDR
		//UpdateColorSpace();

		//DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
		//Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain3;
		//if (SUCCEEDED(m_swapChain->GetDxSwapChain()->QueryInterface(__uuidof(IDXGISwapChain3), &swapChain3)))
		//{
		//	UINT colorSpaceSupport = 0;
		//	if (SUCCEEDED(swapChain3->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport))
		//		&& (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
		//	{
		//		DX::ThrowIfFailed(swapChain3->SetColorSpace1(colorSpace));
		//	}
		//}

		// Create a render target view of the swap chain back buffer.
		DX::ThrowIfFailed(m_swapChain->GetDxSwapChain()->GetBuffer(0, IID_PPV_ARGS(m_defaultD3dRenderTarget.ReleaseAndGetAddressOf())));

		CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, FormatHelper::ToFormat(m_presentationParameters.GetBackBufferFormat()));
		DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(
			m_defaultD3dRenderTarget.Get(),
			&renderTargetViewDesc,
			m_d3dDefaultRenderTargetView.ReleaseAndGetAddressOf()
		));

		m_defaultD3dDepthStencilView = nullptr;
		if (m_presentationParameters.GetDepthStencilFormat() != DepthFormat::None)
		{
			m_depthStencilBuffer.reset(new DepthStencilBuffer(this, backBufferWidth, backBufferHeight, m_presentationParameters.GetDepthStencilFormat()));
			m_defaultD3dDepthStencilView = m_depthStencilBuffer->m_depthStencilView.Get();
		}
		m_currentD3dDepthStencilView = m_defaultD3dDepthStencilView;

		m_currentD3dRenderTargets[0] = m_d3dDefaultRenderTargetView.Get();
		m_d3dContext->OMSetRenderTargets(1, m_currentD3dRenderTargets.data(), m_currentD3dDepthStencilView);

		// Set the 3D rendering viewport to target the entire window.
		m_viewport = CD3D11_VIEWPORT(
			0.0f,
			0.0f,
			static_cast<float>(backBufferWidth),
			static_cast<float>(backBufferHeight)
		);

		// Set the viewport.
		m_d3dContext->RSSetViewports(1, m_viewport.Get11());
#endif
	}
	
	void GraphicsDevice::DrawIndexedPrimitives(PrimitiveType primitiveType, uint32_t baseVertex, uint32_t startIndex, uint32_t primitiveCount)
	{
#ifdef TRIO_DIRECTX
		ApplyState(true);

		m_d3dContext->IASetPrimitiveTopology(FormatHelper::FormatToPrimitive(primitiveType));

		m_textureCollection->BindAllTextures(this);
		m_samplerCollection->BindAllSamplers(this);

		uint32_t indexCount = FormatHelper::GetElementCountArray(primitiveType, primitiveCount);
		m_d3dContext->DrawIndexed(indexCount, startIndex, baseVertex);
#endif
	}

	void GraphicsDevice::Reset(PresentationParameters presentationParameters)
	{
		m_presentationParameters = presentationParameters;
		CreateWindowSizeDependentResources();
	}

	void GraphicsDevice::Reset(PresentationParameters presentationParameters, GraphicsAdapter* graphicsAdapter)
	{
		if (m_adapter != graphicsAdapter)
		{
			//TO-DO:
		}
		m_adapter = graphicsAdapter;
		m_presentationParameters = presentationParameters;
		CreateWindowSizeDependentResources();
	}

	void GraphicsDevice::HandleDeviceLost()
	{
		DeviceLost();

		m_depthStencilBuffer.reset();
		m_defaultD3dDepthStencilView = nullptr;
		m_currentD3dDepthStencilView = nullptr;

		m_d3dDefaultRenderTargetView.Reset();

		m_defaultD3dRenderTarget.Reset();
		m_swapChain.reset();
		m_d3dContext.Reset();
		m_d3dAnnotation.Reset();

#ifdef _DEBUG
		{
			Microsoft::WRL::ComPtr<ID3D11Debug> d3dDebug;
			if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
			{
				d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
			}
		}
#endif
		m_d3dDevice.Reset();
		m_dxgiFactory.Reset();

		CreateDeviceResources();
		CreateWindowSizeDependentResources();

		DeviceRestored();
	}

	void GraphicsDevice::Flush()
	{
#ifdef TRIO_DIRECTX
		m_d3dContext->Flush();
#endif
	}

	void GraphicsDevice::Present()
	{
#ifdef TRIO_DIRECTX
		DeviceErrorStatus hr = DeviceErrorStatus::Fail;
		//if (m_options & c_AllowTearing)
		//{
		//	// Recommended to always use tearing if supported when using a sync interval of 0.
		//	hr = m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
		//}
		//else
		//{
			// The first argument instructs DXGI to block until VSync, putting the application
			// to sleep until the next VSync. This ensures we don't waste any cycles rendering
			// frames that will never be displayed to the screen.
			//var syncInterval = PresentationParameters.PresentationInterval.GetSyncInterval();
			hr = m_swapChain->Present(1, 0);
		//}

		 // Discard the contents of the render target.
		// This is a valid operation only when the existing contents will be entirely
		// overwritten. If dirty or scroll rects are used, this call should be removed.
		m_d3dContext->DiscardView(m_d3dDefaultRenderTargetView.Get());

		if (m_depthStencilBuffer != nullptr && m_depthStencilBuffer->GetDepthStencilView())
		{
			// Discard the contents of the depth stencil.
			m_d3dContext->DiscardView(m_depthStencilBuffer->GetDepthStencilView());
		}

		// If the device was removed either by a disconnection or a driver upgrade, we 
		// must recreate all device resources.
		if (hr == DeviceErrorStatus::DeviceRemoved || hr == DeviceErrorStatus::DeviceReset)
		{
#ifdef _DEBUG
			char buff[64] = {};
			sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
				static_cast<unsigned int>((hr == DeviceErrorStatus::DeviceRemoved) ? m_d3dDevice->GetDeviceRemovedReason() : (long)hr));
			OutputDebugStringA(buff);
#endif
			HandleDeviceLost();
		}
		else
		{
			DX::ThrowIfFailed(hr);

			if (!m_dxgiFactory->IsCurrent())
			{
				// Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
				CreateFactory();
			}
		}
#endif
	}

	void GraphicsDevice::SetBlendState(BlendState* state)
	{
		if (m_blendState == state)
			return;

		m_blendState = state;
		m_blendStateDirty = true;
	}

	void GraphicsDevice::SetConstantBuffer(ShaderStage stage, int slot, ConstantBuffer* buffer)
	{
		switch (stage)
		{
		case ShaderStage::Vertex:
			(*m_vertexConstantBuffers)[slot] = buffer;
			break;
		case ShaderStage::Pixel:
			(*m_pixelConstantBuffers)[slot] = buffer;
			break;
		case ShaderStage::Geometry:
			break;
		case ShaderStage::Compute:
			break;
		default:
			break;
		}
	}

	void GraphicsDevice::SetDepthStencilState(DepthStencilState* state)
	{
		if (m_depthStencilState == state)
			return;

		m_depthStencilState = state;
		m_depthStencilStateDirty = true;
	}

	void GraphicsDevice::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
		if (m_indexBuffer == indexBuffer)
			return;

		m_indexBuffer = indexBuffer;
		m_indexBufferDirty = true;
	}

	void GraphicsDevice::SetRasterizerState(RasterizerState* state)
	{
		if (m_rasterizerState == state)
			return;

		m_rasterizerState = state;
		m_rasterizerStateDirty = true;
	}

	void GraphicsDevice::SetRenderTarget(RenderTarget2D* renderTarget)
	{
		if (renderTarget == nullptr)
		{
			RenderTargetBindings bindings;

			SetRenderTargets(bindings);
		}
		else
		{
			RenderTargetBindings bindings(RenderTargetBinding(dynamic_cast<Texture*>(renderTarget)));

			SetRenderTargets(bindings);
		}
	}

	void GraphicsDevice::SetRenderTargets(RenderTargetBindings& bindings)
	{
		if (m_renderTargetBindings.size() == bindings.size())
		{
			bool areTheSame = true;
			for (int i = 0; i < m_renderTargetBindings.size(); i++)
			{
				if (m_renderTargetBindings[i].RenderTarget != bindings[i].RenderTarget ||
					m_renderTargetBindings[i].ArraySlice != bindings[i].ArraySlice)
				{
					areTheSame = false;
					break;
				}
			}

			if (areTheSame)
				return;
		}

		m_renderTargetBindings.clear();

		if (bindings.size() == 0)
		{
			//m_currentD3dRenderTargets = std::vector<ID3D11RenderTargetView*>(4, nullptr);

			m_currentD3dDepthStencilView = m_defaultD3dDepthStencilView;
			m_currentD3dRenderTargets[0] = m_d3dDefaultRenderTargetView.Get();
			m_d3dContext->OMSetRenderTargets(1, &m_currentD3dRenderTargets[0], m_currentD3dDepthStencilView);
		}
		else
		{
			//m_currentD3dRenderTargets = std::vector<ID3D11RenderTargetView*>(bindings.size(), nullptr);

			for (size_t i = 0; i < bindings.size(); i++)
			{
				const RenderTargetBinding& binding = bindings[i];
				IRenderTarget* target = dynamic_cast<IRenderTarget*>(binding.RenderTarget);
				m_currentD3dRenderTargets[i] = target->GetRenderTargetView(binding.ArraySlice);

				if (i == 0) {
					m_currentD3dDepthStencilView = target->GetDepthStencilView();
				}
			}

			m_d3dContext->OMSetRenderTargets(bindings.size(), &m_currentD3dRenderTargets[0], m_currentD3dDepthStencilView);

			m_renderTargetBindings = bindings;
		}

		//viewport = ?
	}

	void GraphicsDevice::SetSamplerState(int index, SamplerState* state)
	{
		m_samplerCollection->SetSampler(index, state);
	}

	void GraphicsDevice::SetVertexBuffer(VertexBuffer* buffer)
	{
		if (m_vertexBindings.size() == 1 && m_vertexBindings[0].Buffer == buffer ||
			m_vertexBindings.size() == 0 && buffer == nullptr)
			return;

		m_vertexBufferDirty = true;

		if (buffer == nullptr)
		{
			m_vertexBindings.clear();
			return;
		}
		//TODO:
		m_vertexBindings.clear();
		m_vertexBindings[0].Buffer = buffer;
		m_vertexBindings[0].Offset = 0;
		m_vertexBindings[0].Stride = buffer->GetVertexDeclaration()->GetVertexStride();

#ifdef TRIO_DIRECTX
		m_vertexBuffers[0] = buffer->m_buffer.Get();
		m_vertexOffsets[0] = 0;
		m_vertexStrides[0] = m_vertexBindings[0].Stride;
#endif
	}

	void GraphicsDevice::SetVertexBuffer(VertexBufferBindings &bindings)
	{
		if (AreSameVertexBindings(bindings))
			return;

		m_vertexBindings = bindings;
#ifdef TRIO_DIRECTX
		for (int i = 0; i < m_vertexBindings.size(); i++)
		{
			m_vertexBuffers[i] = m_vertexBindings[i].Buffer->m_buffer.Get();
			m_vertexOffsets[i] = m_vertexBindings[i].Offset;
			m_vertexStrides[i] = m_vertexBindings[i].Stride;
		}
#endif
		m_vertexBufferDirty = true;
	}

	void GraphicsDevice::SetPixelShader(Shader* shader)
	{
		if (m_pixelShader == shader)
			return;

		m_pixelShader = shader;
		m_pixelShaderDirty = true;
	}

	void GraphicsDevice::SetVertexShader(Shader* shader)
	{
		if (m_vertexShader == shader)
			return;

		m_vertexShader = shader;
		m_vertexShaderDirty = true;
	}

	void GraphicsDevice::SetViewport(Viewport viewport)
	{
		m_viewport = viewport;
#ifdef TRIO_DIRECTX
		m_d3dContext->RSSetViewports(1, m_viewport.Get11());
#endif
	}
}