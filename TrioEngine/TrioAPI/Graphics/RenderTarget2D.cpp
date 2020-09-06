#include "stdafx.h"
#include "RenderTarget2D.h"

#include "FormatHelper.h"
#include "BindFlags.h"
#include "ResourceOptionFlags.h"
#include "CpuAccessFlags.h"
#include "DepthStencilBuffer.h"

namespace TrioEngine
{
	RenderTarget2D::RenderTarget2D(GraphicsDevice* graphicsDevice, int width, int height)
		: Texture2D(graphicsDevice, width, height, 1, SurfaceFormat::Color, SurfaceType::RenderTarget, 1, 0, false), m_initialized(false), m_depthFormat(DepthFormat::None), m_renderTargetUsage(RenderTargetUsage::DiscardContents),
		m_depthStencilBuffer(nullptr)
	{
		CommonConstructor();
	}

	RenderTarget2D::RenderTarget2D(GraphicsDevice* graphicsDevice, int width, int height, SurfaceFormat preferredFormat) :
		Texture2D(graphicsDevice, width, height, 1, preferredFormat, SurfaceType::RenderTarget, 1, 0, false), m_initialized(false), m_depthFormat(DepthFormat::None), m_renderTargetUsage(RenderTargetUsage::DiscardContents),
		m_depthStencilBuffer(nullptr)
	{
		CommonConstructor();
	}

	RenderTarget2D::RenderTarget2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat) :
		Texture2D(graphicsDevice, width, height, 1, preferredFormat, SurfaceType::RenderTarget, 1, 0, false), m_initialized(false), m_depthFormat(preferredDepthFormat), m_renderTargetUsage(RenderTargetUsage::DiscardContents),
		m_depthStencilBuffer(nullptr)
	{
		CommonConstructor();
	}

	RenderTarget2D::RenderTarget2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmapLevels, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, RenderTargetUsage usage) :
		Texture2D(graphicsDevice, width, height, 1, preferredFormat, SurfaceType::RenderTarget, 1, 0, false), m_initialized(false), m_depthFormat(preferredDepthFormat), m_renderTargetUsage(usage),
		m_depthStencilBuffer(nullptr)
	{
		CommonConstructor();
	}

	RenderTarget2D::~RenderTarget2D()
	{
#if TRIO_DIRECTX
		for (size_t i = 0; i < m_renderTargetViews.size(); i++)
		{
			auto rtView = m_renderTargetViews[i];
			RELEASE_COM(rtView);
		}
		m_renderTargetViews.clear();
#endif

		Texture2D::~Texture2D();
	}

	void RenderTarget2D::CommonConstructor()
{
		if (m_initialized) return;

		m_initialized = true;

#if TRIO_DIRECTX
		m_renderTargetViews.clear();
		if (m_arraySize == 1)
		{
			CreateTexture(nullptr);
			ID3D11RenderTargetView* renderTargetView = nullptr;

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			renderTargetViewDesc.Format = m_tex2DDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			DX::ThrowIfFailed(m_device->GetD3DDevice()->CreateRenderTargetView(m_texture, &renderTargetViewDesc, &renderTargetView));

			m_renderTargetViews.push_back(renderTargetView);
		}
		else
		{
			CreateTexture(nullptr);
			m_renderTargetViews.resize(m_arraySize);
			//TODO:
			for (size_t i = 0; i < m_arraySize; i++)
			{
				ID3D11RenderTargetView* renderTargetView = nullptr;

				D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
				renderTargetViewDesc.Format = ToFormat(m_format);
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				renderTargetViewDesc.Texture2D.MipSlice = 0;
				DX::ThrowIfFailed(m_device->GetD3DDevice()->CreateRenderTargetView(m_texture, &renderTargetViewDesc, &renderTargetView));

				m_renderTargetViews.push_back(renderTargetView);

				/*D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = format;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
				shaderResourceViewDesc.Texture2D.MipLevels = 1;
				hr = Device->CreateShaderResourceView(_Texture2D, &shaderResourceViewDesc, &_ShaderResourceView);*/
			}
		}

		if (m_depthFormat == DepthFormat::None)
			return;

		m_depthStencilBuffer.reset(new DepthStencilBuffer(m_device, m_width, m_height, m_depthFormat));
#endif
	}


	int RenderTarget2D::GetWidth()
	{
		return m_width;
	}

	int RenderTarget2D::GetHeight()
	{
		return m_height;
	}

#if TRIO_DIRECTX
	ID3D11RenderTargetView* RenderTarget2D::GetRenderTargetView(int arraySlice)
	{
		return m_renderTargetViews[arraySlice];
	}

	ID3D11DepthStencilView* RenderTarget2D::GetDepthStencilView()
	{
		if (m_depthStencilBuffer == nullptr) return nullptr;

		return m_depthStencilBuffer->GetDepthStencilView();
	}
#endif
}