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

namespace Vago
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
		m_pCurrentDepthStencilBuffer(nullptr),

		m_pVertexTextureCollection(nullptr),

		m_pTextureCollection(nullptr),
		m_pSamplerCollection(nullptr),
		m_pAdapter(adapter),
		m_presentationParameters(parameters),
		m_uiBackBufferCount(2),
		m_pVertexShader(nullptr),
		m_bVertexShaderDirty(false),

		m_pVertexConstantBuffers(nullptr),
		m_pPixelConstantBuffers(nullptr),

		m_pPixelShader(nullptr),
		m_bPixelShaderDirty(false),

		m_bRasterizerStateDirty(false),
		m_pRasterizerState(nullptr),

		m_bBlendStateDirty(false),
		m_pBlendState(nullptr),

		m_bDepthStencilStateDirty(false),
		m_pDepthStencilState(nullptr)
	{
		CreateDeviceResources();
		CreateWindowSizeDependentResources();

		DepthStencilState::InitStates();
		RasterizerState::InitStates();
		BlendState::InitStates();
		SamplerState::InitStates();

		m_pVertexTextureCollection = new TextureCollection(ShaderStage::Vertex);

		m_pTextureCollection = new TextureCollection(ShaderStage::Pixel);
		m_pSamplerCollection = new SamplerStateCollection(ShaderStage::Pixel);

		m_pVertexConstantBuffers = new ConstantBufferCollection(ShaderStage::Vertex);
		m_pPixelConstantBuffers = new ConstantBufferCollection(ShaderStage::Pixel);

		m_bIndexBufferDirty = true;
		m_bVertexBufferDirty = true;

#ifdef TRIO_DIRECTX
		for (size_t i = 0; i < MaxVertexBuffers; i++)
		{
			m_aVertexBuffers[i] = nullptr;
			m_aVertexOffsets[i] = 0;
			m_aVertexStrides[i] = 0;
		}
#endif
	}

	GraphicsDevice::~GraphicsDevice()
	{
	}

	void GraphicsDevice::ApplyState(bool applyShaders)
	{
		if (m_bBlendStateDirty)
		{
			m_pBlendState->ApplyState(this);
			m_bBlendStateDirty = false;
		}
		if (m_bDepthStencilStateDirty)
		{
			m_pDepthStencilState->ApplyState(this);
			m_bDepthStencilStateDirty = false;
		}
		if (m_bRasterizerStateDirty)
		{
			m_pRasterizerState->ApplyState(this);
			m_bRasterizerStateDirty = false;
		}

		if (!applyShaders)
		{
			return;
		}

		if (m_bIndexBufferDirty)
		{
			if (m_pIndexBuffer)
			{
#ifdef TRIO_DIRECTX
				m_d3dContext->IASetIndexBuffer(m_pIndexBuffer->m_pBuffer, ToFormat(m_pIndexBuffer->m_eElementSize), 0);
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
			m_bIndexBufferDirty = false;
		}

		if (m_bVertexBufferDirty)
		{
			if (m_vVertexBindings.size() > 0)
			{
#ifdef TRIO_DIRECTX
				m_d3dContext->IASetVertexBuffers(0, m_vVertexBindings.size(), &m_aVertexBuffers[0], m_aVertexStrides, m_aVertexOffsets);
#endif
			}
			else
			{
#ifdef TRIO_DIRECTX
				m_d3dContext->IASetVertexBuffers(0, 0, nullptr, 0, 0);
#endif
			}
		}

		if (m_bVertexShaderDirty)
		{
#ifdef TRIO_DIRECTX
			if (m_pVertexShader != nullptr)
			{
				m_d3dContext->VSSetShader(m_pVertexShader->m_uShaderPtr.m_pVertexShader, nullptr, 0);
			}
			else
			{
				m_d3dContext->VSSetShader(nullptr, nullptr, 0);
			}
#endif
		}

		if (m_bVertexShaderDirty || m_bVertexBufferDirty)
		{
#ifdef TRIO_DIRECTX
			ID3D11InputLayout* layout = m_pVertexShader->GetInputLayouts()->Get(m_vVertexBindings[0].Buffer->m_pVertexDeclaration);
			m_d3dContext->IASetInputLayout(layout);
#elif TRIO_OPENGL

#endif
			m_bVertexShaderDirty = false;
			m_bVertexBufferDirty = false;
		}

		if (m_bPixelShaderDirty)
		{
#ifdef TRIO_DIRECTX
			if (m_pPixelShader)
			{
				m_d3dContext->PSSetShader(m_pPixelShader->m_uShaderPtr.m_pPixelShader, nullptr, 0);
			}
			else
			{
				m_d3dContext->PSSetShader(nullptr, nullptr, 0);
			}
#endif
			m_bPixelShaderDirty = false;
		}

#ifdef TRIO_DIRECTX
		m_pVertexConstantBuffers->SetConstantBuffers(this);
		m_pPixelConstantBuffers->SetConstantBuffers(this);
#endif
	}

	bool GraphicsDevice::AreSameVertexBindings(VertexBufferBindings &bindings)
	{
		if (bindings.size() != m_vVertexBindings.size())
			return false;

		for (int i = 0; i < bindings.size(); i++)
		{
			if (bindings[i].Buffer != m_vVertexBindings[i].Buffer ||
				bindings[i].Offset != m_vVertexBindings[i].Offset ||
				bindings[i].Stride != m_vVertexBindings[i].Stride)
			{
				return false;
			}
		}
		return true;
	}

	void GraphicsDevice::Clear(Color &color)
	{
		ClearOptions options = ClearOptions::Target;

		if (m_depthStencilBuffer != nullptr)
		{
			options = options | ClearOptions::DepthBuffer;
			if (HasStencil(m_depthStencilBuffer->GetDepthFormat()))
				options = options | ClearOptions::Stencil;
		}

		Clear(options, color, m_stScreenViewport.MaxDepth, 0);
	}

	void GraphicsDevice::Clear(ClearOptions options, Color &color, float depth, uint8_t stencil)
	{
#ifdef TRIO_DIRECTX
		if ((options & ClearOptions::Target) == ClearOptions::Target)
		{
			const float* colorFloat = reinterpret_cast<const float*>(&color);
			for (int i = 0; i < m_vCurrentRenderTargets.size() && m_vCurrentRenderTargets[i] != nullptr; i++)
			{
				m_d3dContext->ClearRenderTargetView(m_vCurrentRenderTargets[i], colorFloat);
			}
		}
		uint32_t flags = 0;

		if ((options & ClearOptions::DepthBuffer) == ClearOptions::DepthBuffer)
			flags |= D3D11_CLEAR_DEPTH;

		if ((options & ClearOptions::Stencil) == ClearOptions::Stencil)
			flags |= D3D11_CLEAR_STENCIL;

		if (flags != 0 && m_depthStencilBuffer != nullptr)
		{
			m_d3dContext->ClearDepthStencilView(m_depthStencilBuffer->GetDepthStencilView(), flags, depth, stencil);
		}
#endif
	}

	void GraphicsDevice::CreateDeviceResources()
	{
#ifdef TRIO_DIRECTX
		// This flag adds support for surfaces with a different color channel ordering
		// than the API default. It is required for compatibility with Direct2D.
		// D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		UINT creationFlags = 0;

#if defined(_DEBUG)
		if (SdkLayersAvailable())
		{
			// If the project is in a debug build, enable debugging via SDK Layers with this flag.
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}
#endif
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

		IDXGIAdapter1* adapter = m_pAdapter->GetD3DAdapter();

		D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		// Create the Direct3D 11 API device object and a corresponding context.
		HRESULT hr = E_FAIL;
		if (adapter)
		{
			hr = D3D11CreateDevice(
				adapter,
				D3D_DRIVER_TYPE_UNKNOWN,
				0,
				creationFlags,
				s_featureLevels,
				ARRAYSIZE(s_featureLevels),
				D3D11_SDK_VERSION,
				m_d3dDevice.ReleaseAndGetAddressOf(),   // Returns the Direct3D device created.
				&m_d3dFeatureLevel,                     // Returns feature level of device created.
				m_d3dContext.ReleaseAndGetAddressOf()   // Returns the device immediate context.
			);

			if (hr == E_INVALIDARG)
			{
				assert(s_featureLevels[0] == D3D_FEATURE_LEVEL_11_1);

				// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
				hr = D3D11CreateDevice(
					adapter,
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					creationFlags,
					&s_featureLevels[1],
					ARRAYSIZE(s_featureLevels) - 1,
					D3D11_SDK_VERSION,
					m_d3dDevice.ReleaseAndGetAddressOf(),
					&m_d3dFeatureLevel,
					m_d3dContext.ReleaseAndGetAddressOf()
				);
			}
		}
		if (FAILED(hr))
		{
			// If the initialization fails, fall back to the WARP device.
			// For more information on WARP, see: 
			// http://go.microsoft.com/fwlink/?LinkId=286690
			hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
				0,
				creationFlags,
				s_featureLevels,
				ARRAYSIZE(s_featureLevels),
				D3D11_SDK_VERSION,
				m_d3dDevice.ReleaseAndGetAddressOf(),
				&m_d3dFeatureLevel,
				m_d3dContext.ReleaseAndGetAddressOf()
			);

			if (hr == E_INVALIDARG)
			{
				assert(s_featureLevels[0] == D3D_FEATURE_LEVEL_11_1);

				// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
				hr = D3D11CreateDevice(
					nullptr,
					D3D_DRIVER_TYPE_WARP,
					nullptr,
					creationFlags,
					&s_featureLevels[1],
					ARRAYSIZE(s_featureLevels) - 1,
					D3D11_SDK_VERSION,
					m_d3dDevice.ReleaseAndGetAddressOf(),
					&m_d3dFeatureLevel,
					m_d3dContext.ReleaseAndGetAddressOf()
				);
			}

			if (SUCCEEDED(hr))
			{
				OutputDebugStringA("Direct3D Adapter - WARP\n");
			}
		}

		DX::ThrowIfFailed(hr);

#ifndef NDEBUG
		Microsoft::WRL::ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
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
		if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
		{
			(void)m_d3dContext.As(&m_d3dContext1);
			(void)m_d3dContext.As(&m_d3dAnnotation);
		}
#endif
	}

	void GraphicsDevice::CreateWindowSizeDependentResources()
	{
#ifdef TRIO_DIRECTX
		if (!m_presentationParameters.GetHostHWND())
		{
			throw std::exception("Call SetWindow with a valid Win32 window handle");
		}

		// Clear the previous window size specific context.
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
		m_vCurrentRenderTargets = std::vector<ID3D11RenderTargetView*>(4, nullptr);

		m_depthStencilBuffer.reset();
		m_d3dRenderTargetView.Reset();
		m_d3dContext->Flush();

		// Determine the render target size in pixels.
		uint32_t backBufferWidth = std::max<uint32_t>(m_presentationParameters.GetBackBufferWidth(), 1);
		uint32_t backBufferHeight = std::max<uint32_t>(m_presentationParameters.GetBackBufferHeight(), 1);

		if (m_swapChain)
		{
			// If the swap chain already exists, resize it.
			HRESULT hr = m_swapChain->ResizeBuffers(
				m_uiBackBufferCount,
				backBufferWidth,
				backBufferHeight,
				ToFormat(m_presentationParameters.GetBackBufferFormat()),
				0
			);

			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
#ifdef _DEBUG
				char buff[64] = {};
				sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
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
				DX::ThrowIfFailed(hr);
			}
		}
		else
		{
			// Otherwise, create a new one using the same adapter as the existing Direct3D device.

			// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
			Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
			DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

			Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

			Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
			DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

			Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory2;
			if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
			{
				// DirectX 11.1 or later
				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = backBufferWidth;
				swapChainDesc.Height = backBufferHeight;
				swapChainDesc.Format = ToFormat(m_presentationParameters.GetBackBufferFormat());
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = m_uiBackBufferCount;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

				DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
				fsSwapChainDesc.Windowed = TRUE;

				// Create a SwapChain from a Win32 window.
				DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
					m_d3dDevice.Get(),
					m_presentationParameters.GetHostHWND(),
					&swapChainDesc,
					&fsSwapChainDesc,
					nullptr, m_swapChain1.ReleaseAndGetAddressOf()
				));

				DX::ThrowIfFailed(m_swapChain1.As(&m_swapChain));
			}
			else
			{
				// DirectX 11.0
				DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
				swapChainDesc.BufferDesc.Width = backBufferWidth;
				swapChainDesc.BufferDesc.Height = backBufferHeight;
				swapChainDesc.BufferDesc.Format = ToFormat(m_presentationParameters.GetBackBufferFormat());
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = m_uiBackBufferCount;
				swapChainDesc.OutputWindow = m_presentationParameters.GetHostHWND();
				swapChainDesc.Windowed = TRUE;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

				DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(
					m_d3dDevice.Get(),
					&swapChainDesc,
					m_swapChain.ReleaseAndGetAddressOf()
				));
			}

			// This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
			DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_presentationParameters.GetHostHWND(), DXGI_MWA_NO_ALT_ENTER));
		}

		// Create a render target view of the swap chain back buffer.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

		DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			m_d3dRenderTargetView.ReleaseAndGetAddressOf()
		));

		if (m_presentationParameters.GetDepthStencilFormat() != DepthFormat::None)
		{
			m_depthStencilBuffer.reset(new DepthStencilBuffer(this, backBufferWidth, backBufferHeight, m_presentationParameters.GetDepthStencilFormat()));
		}

		m_vCurrentRenderTargets[0] = m_d3dRenderTargetView.Get();
		m_d3dContext->OMSetRenderTargets(1, &m_vCurrentRenderTargets[0], m_depthStencilBuffer->m_depthStencilView.Get());

		// Set the 3D rendering viewport to target the entire window.
		m_stScreenViewport = CD3D11_VIEWPORT(
			0.0f,
			0.0f,
			static_cast<float>(backBufferWidth),
			static_cast<float>(backBufferHeight)
		);

		// Set the viewport.
		m_d3dContext->RSSetViewports(1, &m_stScreenViewport);

#endif
	}

	void GraphicsDevice::DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount)
	{
#ifdef TRIO_DIRECTX
		ApplyState(true);

		m_d3dContext->IASetPrimitiveTopology(FormatToPrimitive(primitiveType));
		
		m_pTextureCollection->BindAllTextures(this);
		m_pSamplerCollection->BindAllSamplers(this);

		int indexCount = GetElementCountArray(primitiveType, primitiveCount);
		m_d3dContext->DrawIndexed(indexCount, startIndex, baseVertex);
#endif
	}
	
	void GraphicsDevice::DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int primitiveCount)
	{
#ifdef TRIO_DIRECTX

#endif
	}

	void GraphicsDevice::Reset(PresentationParameters presentationParameters)
	{
		m_presentationParameters = presentationParameters;
		CreateWindowSizeDependentResources();
	}

	void GraphicsDevice::Reset(PresentationParameters presentationParameters, GraphicsAdapter* graphicsAdapter)
	{
		if (m_pAdapter != graphicsAdapter)
		{
			//TO-DO:
		}
		m_pAdapter = graphicsAdapter;
		m_presentationParameters = presentationParameters;
		CreateWindowSizeDependentResources();
	}

	void GraphicsDevice::HandleDeviceLost()
	{
		DeviceLost();

		m_depthStencilBuffer.reset();
		m_d3dRenderTargetView.Reset();
		m_swapChain.Reset();
		m_swapChain1.Reset();
		m_d3dContext.Reset();
		m_d3dContext1.Reset();
		m_d3dAnnotation.Reset();
		m_d3dDevice1.Reset();

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

		CreateDeviceResources();
		CreateWindowSizeDependentResources();

		DeviceRestored();
	}

	void GraphicsDevice::Present()
	{
#ifdef TRIO_DIRECTX
		// The first argument instructs DXGI to block until VSync, putting the application
		// to sleep until the next VSync. This ensures we don't waste any cycles rendering
		// frames that will never be displayed to the screen.
		HRESULT hr = m_swapChain->Present(1, 0);

		if (m_d3dContext1)
		{
			// Discard the contents of the render target.
			// This is a valid operation only when the existing contents will be entirely
			// overwritten. If dirty or scroll rects are used, this call should be removed.
			m_d3dContext1->DiscardView(m_d3dRenderTargetView.Get());

			if (m_depthStencilBuffer->GetDepthStencilView())
			{
				// Discard the contents of the depth stencil.
				m_d3dContext1->DiscardView(m_depthStencilBuffer->GetDepthStencilView());
			}
		}

		// If the device was removed either by a disconnection or a driver upgrade, we 
		// must recreate all device resources.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
#ifdef _DEBUG
			char buff[64] = {};
			sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
			OutputDebugStringA(buff);
#endif
			HandleDeviceLost();
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
#endif
	}

	void GraphicsDevice::SetBlendState(BlendState* state)
	{
		if (m_pBlendState == state)
			return;

		m_pBlendState = state;
		m_bBlendStateDirty = true;
	}

	void GraphicsDevice::SetConstantBuffer(ShaderStage stage, int slot, ConstantBuffer* buffer)
	{
		switch (stage)
		{
		case ShaderStage::Vertex:
			(*m_pVertexConstantBuffers)[slot] = buffer;
			break;
		case ShaderStage::Pixel:
			(*m_pPixelConstantBuffers)[slot] = buffer;
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
		if (m_pDepthStencilState == state)
			return;

		m_pDepthStencilState = state;
		m_bDepthStencilStateDirty = true;
	}

	void GraphicsDevice::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
		if (m_pIndexBuffer == indexBuffer)
			return;

		m_pIndexBuffer = indexBuffer;
		m_bIndexBufferDirty = true;
	}

	void GraphicsDevice::SetRasterizerState(RasterizerState* state)
	{
		if (m_pRasterizerState == state)
			return;

		m_pRasterizerState = state;
		m_bRasterizerStateDirty = true;
	}

	void GraphicsDevice::SetSamplerState(int index, SamplerState* state)
	{
		m_pSamplerCollection->SetSampler(index, state);
	}

	void GraphicsDevice::SetVertexBuffer(VertexBuffer* buffer)
	{
		if (m_vVertexBindings.size() == 1 && m_vVertexBindings[0].Buffer == buffer ||
			m_vVertexBindings.size() == 0 && buffer == nullptr)
			return;

		m_bVertexBufferDirty = true;

		if (buffer == nullptr)
		{
			m_vVertexBindings.clear();
			return;
		}
		//TODO:
		m_vVertexBindings.clear();
		m_vVertexBindings[0].Buffer = buffer;
		m_vVertexBindings[0].Offset = 0;
		m_vVertexBindings[0].Stride = buffer->GetVertexDeclaration()->GetVertexStride();

#ifdef TRIO_DIRECTX
		m_aVertexBuffers[0] = buffer->m_pBuffer;
		m_aVertexOffsets[0] = 0;
		m_aVertexStrides[0] = m_vVertexBindings[0].Stride;
#endif
	}

	void GraphicsDevice::SetVertexBuffer(VertexBufferBindings &bindings)
	{
		if (AreSameVertexBindings(bindings))
			return;

		m_vVertexBindings = bindings;
#ifdef TRIO_DIRECTX
		for (int i = 0; i < m_vVertexBindings.size(); i++)
		{
			m_aVertexBuffers[i] = m_vVertexBindings[i].Buffer->m_pBuffer;
			m_aVertexOffsets[i] = m_vVertexBindings[i].Offset;
			m_aVertexStrides[i] = m_vVertexBindings[i].Stride;
		}
#endif
		m_bVertexBufferDirty = true;
	}

	void GraphicsDevice::SetPixelShader(Shader* shader)
	{
		if (m_pPixelShader == shader)
			return;

		m_pPixelShader = shader;
		m_bPixelShaderDirty = true;
	}

	void GraphicsDevice::SetVertexShader(Shader* shader)
	{
		if (m_pVertexShader == shader)
			return;

		m_pVertexShader = shader;
		m_bVertexShaderDirty = true;
	}
}