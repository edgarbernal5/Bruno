#include "stdafx.h"
#include "DepthStencilBuffer.h"

#include "GraphicsDevice.h"
#include "FormatHelper.h"

namespace TrioEngine
{
	DepthStencilBuffer::DepthStencilBuffer() :
#ifdef TRIO_DIRECTX
		//m_depthStencilBuffer(nullptr),
		m_depthStencilView(nullptr),
		m_shaderResourceView(nullptr),
#elif TRIO_OPENGL
		: m_glDepthBuffer(0),
#endif
		m_device(nullptr)
	{

	}

	DepthStencilBuffer::DepthStencilBuffer(GraphicsDevice* device, int width, int height, DepthFormat format) :
		m_depthFormat(format),

#ifdef TRIO_DIRECTX
		m_shaderResourceView(nullptr),
#elif TRIO_OPENGL
		m_glDepthBuffer(0),
		m_glStencilBuffer(0),
#endif
		m_device(device) {

#ifdef TRIO_DIRECTX

		// Create a depth stencil view for use with 3D rendering if needed.
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			ToFormat(format),
			width,
			height,
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
		);

		DX::ThrowIfFailed(device->GetD3DDevice()->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			m_depthStencil.GetAddressOf()
		));

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateDepthStencilView(
			m_depthStencil.Get(),
			&depthStencilViewDesc,
			m_depthStencilView.ReleaseAndGetAddressOf()
		));

#endif
	}

#ifdef TRIO_DIRECTX
	DepthStencilBuffer::DepthStencilBuffer(GraphicsDevice* device, int width, int height, DepthFormat format, DepthStencilViewDimension viewDimension, DepthStencilViewFlags viewFlags) {

	}

#endif
	DepthStencilBuffer::~DepthStencilBuffer()
	{
#ifdef TRIO_DIRECTX
		m_depthStencilView.Reset();
		m_depthStencil.Reset();
		//m_depthStencilBuffer.Reset();
#endif
	}
}