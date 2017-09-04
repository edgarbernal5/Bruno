#include "stdafx.h"
#include "RenderTarget2D.h"

#include "GraphicsDevice.h"
#include "DepthStencilBuffer.h"
namespace Cuado
{
	RenderTarget2D::RenderTarget2D() : 
		Texture2D(),
#ifdef TRIO_DIRECTX
		m_RenderTargetView(nullptr),
#elif TRIO_OPENGL
		m_glColorBuffer(0),
#endif
		m_DepthBuffer(nullptr)
	{
	}

	RenderTarget2D::RenderTarget2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipMap, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, uint32_t preferredMultiSampleCount, uint32_t preferredMultiSampleQuality, RenderTargetUsage usage, bool shared) :
		Texture2D(graphicsDevice, width, height, mipMap, preferredFormat, SurfaceType::RenderTarget, preferredMultiSampleCount, preferredMultiSampleQuality, shared),
		m_DepthFormat(preferredDepthFormat),
		m_RenderTargetUsage(usage),

#ifdef TRIO_DIRECTX
		m_RenderTargetView(nullptr),
#elif TRIO_OPENGL
		m_glColorBuffer(0),
#endif
		m_DepthBuffer(nullptr)
	{
#ifdef TRIO_DIRECTX
		if (preferredDepthFormat == DepthFormat::None)
			return;

		m_DepthBuffer = new DepthStencilBuffer(m_pDevice, m_iWidth, m_iHeight, m_DepthFormat, DepthStencilViewDimension::Texture2D, DepthStencilViewFlags::None);

#elif TRIO_OPENGL
		GLuint color = 0;
		GLuint depth = 0;
		GLuint stencil = 0;

		if (preferredMultiSampleCount > 0 /*&& this.framebufferHelper.SupportsBlitFramebuffer*/)
		{
		}

		if (preferredDepthFormat != DepthFormat::None)
		{
			GLenum depthInternalFormat = GL_DEPTH_COMPONENT16;
			GLenum stencilInternalFormat = 0;

			switch (preferredDepthFormat)
			{
			case Cuado::DepthFormat::Depth16:
				depthInternalFormat = GL_DEPTH_COMPONENT16;
				break;
			case Cuado::DepthFormat::Depth24:
				depthInternalFormat = GL_DEPTH_COMPONENT24;
				break;
			case Cuado::DepthFormat::Depth24Stencil8:
				depthInternalFormat = GL_DEPTH24_STENCIL8;
				break;
			case Cuado::DepthFormat::Depth32:
				depthInternalFormat = GL_DEPTH_COMPONENT32;
				break;
			}

			if (depthInternalFormat > 0)
			{
				glGenRenderbuffers(1, &depth);
				glBindRenderbuffer(GL_RENDERBUFFER, depth);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, preferredMultiSampleCount, depthInternalFormat, width, height);
				if (preferredDepthFormat == DepthFormat::Depth24Stencil8)
				{
					//stencil = depth;
				}
			}
			m_DepthBuffer = new DepthStencilBuffer(m_pDevice, m_iWidth, m_iHeight, m_DepthFormat);
			m_DepthBuffer->m_glDepthBuffer = depth;
			m_DepthBuffer->m_glStencilBuffer = stencil;

			if (color > 0)
			{
				m_glColorBuffer = color;
			}
			else
			{
				m_glColorBuffer = m_glTexture;
			}

		}
#endif

		
	}

	RenderTarget2D::~RenderTarget2D()
	{
#ifdef TRIO_DIRECTX
		ReleaseCOM(m_RenderTargetView);
		if (m_DepthBuffer) {
			delete m_DepthBuffer; 
			m_DepthBuffer = nullptr;
		}
#endif
	}


#ifdef TRIO_DIRECTX
	ID3D11RenderTargetView * RenderTarget2D::GetRenderTargetView(uint32_t arraySlice)
	{
		if (m_pTexture == nullptr)
			CreateTexture(nullptr);

		if (m_RenderTargetView == nullptr)
			m_pDevice->GetD3DDevice()->CreateRenderTargetView(m_pTexture, nullptr, &m_RenderTargetView);

		return m_RenderTargetView;
	}
#endif

	DepthStencilBuffer* RenderTarget2D::GetDepthStencilBuffer()
	{
		return m_DepthBuffer;
	}

	int RenderTarget2D::GetHeight()
	{
		return m_iHeight;
	}

	int RenderTarget2D::GetWidth()
	{
		return m_iWidth;
	}

	RenderTargetUsage RenderTarget2D::GetRenderTargetUsage()
	{
		return m_RenderTargetUsage;
	}
}