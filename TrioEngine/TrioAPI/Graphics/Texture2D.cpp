#include "stdafx.h"
#include "Texture2D.h"

#include "FormatHelper.h"
#include "GraphicsDevice.h"

namespace Cuado
{

	Texture2D::Texture2D() : Texture()
	{
		m_uLevelCount = 1;
	}

	Texture2D::Texture2D(GraphicsDevice *graphicsDevice, std::string filename) :
		Texture(graphicsDevice, SurfaceFormat::Color)
	{
		//
		//HR(D3DX11CreateShaderResourceViewFromFileA(graphicsDevice->GetDevice(), filename.c_str(), 0, 0, &m_ShaderResourceView, 0));
		
#ifdef TRIO_DIRECTX
		m_pShaderResourceView->GetResource(&m_pTexture);
		ID3D11Texture2D* localTexture = reinterpret_cast<ID3D11Texture2D*>(m_pTexture);
		
		localTexture->GetDesc(&m_Tex2DDesc);
		
		m_eFormat = ToSurfaceFormat(m_Tex2DDesc.Format);
		m_uMultiSamples = m_Tex2DDesc.SampleDesc.Count;
		m_uMsQuality = m_Tex2DDesc.SampleDesc.Quality;

		m_iWidth = m_Tex2DDesc.Width;
		m_iHeight = m_Tex2DDesc.Height;
#endif
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format) :
		Texture(graphicsDevice, format)
	{
		CommonConstructor(width, height, mipmap, SurfaceType::TextureType, 1, 0, false);
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height) :
		Texture(graphicsDevice, SurfaceFormat::Color)
	{
		CommonConstructor(width, height, 1, SurfaceType::TextureType, 1, 0, false);
	}
	
	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format, SurfaceType type, uint32_t multiSamples, uint32_t msQuality, bool shared) :
		Texture(graphicsDevice, format),
		m_iWidth(width),
		m_iHeight(height),
		m_eSurfaceType(type),
		m_bShared(shared)
	{
		CommonConstructor(width, height, mipmap, type, multiSamples, msQuality, shared);

	}

	void Texture2D::CommonConstructor(int width, int height, uint32_t mipmap, SurfaceType type, uint32_t multiSamples, uint32_t msQuality, bool shared)
	{
		m_iWidth = width;
		m_iHeight = height;
		m_eSurfaceType = type;
		m_bShared = shared;

		m_uLevelCount = mipmap;
		m_uMultiSamples = multiSamples;
		m_uMsQuality = msQuality;

		m_eUsage = ResourceUsage::Default;


		if (type == SurfaceType::SwapChainRenderTarget)
			return;

		//
#ifdef TRIO_OPENGL
		GLint prevTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);

		m_glTarget = GL_TEXTURE_2D;

		if (m_glTexture == 0)
		{
			glGenTextures(1, &m_glTexture);
			glBindTexture(m_glTarget, m_glTexture);

			GLint wrap = GL_REPEAT;
			// For best compatibility and to keep the default wrap mode of XNA, only set ClampToEdge if either
			// dimension is not a power of two.
			if (((width & (width - 1)) != 0) || ((height & (height - 1)) != 0))
				wrap = GL_CLAMP_TO_EDGE;

			glTexParameteri(m_glTarget, GL_TEXTURE_MIN_FILTER, (m_uLevelCount > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(m_glTarget, GL_TEXTURE_MAG_FILTER, (m_uLevelCount > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_T, wrap);
		}

		GetGLFormat(m_eFormat, m_glInternalFormat, m_glFormat, m_glType);
		//TODO:
		//OJO: el orden de los factores si altera el producto (colocar la siguiente linea luego de bindear)
		glTexImage2D(GL_TEXTURE_2D, 0, m_glInternalFormat, m_iWidth, m_iHeight, 0, m_glFormat, m_glType, nullptr);

		glBindTexture(m_glTarget, prevTexture);
#endif
	}

#ifdef TRIO_DIRECTX
	void Texture2D::CreateTexture(D3D11_SUBRESOURCE_DATA * subdata)
	{
		m_Tex2DDesc = D3D11_TEXTURE2D_DESC();
		m_Tex2DDesc.Usage = (D3D11_USAGE)m_eUsage;
		m_Tex2DDesc.Width = m_iWidth;
		m_Tex2DDesc.Height = m_iHeight;
		m_Tex2DDesc.MipLevels = m_uLevelCount;
		m_Tex2DDesc.Format = ToFormat(m_eFormat);
		m_Tex2DDesc.BindFlags = (uint32_t)BindFlags::ShaderResource;
		m_Tex2DDesc.CPUAccessFlags = 0;
		m_Tex2DDesc.MiscFlags = 0;
		m_Tex2DDesc.ArraySize = 1;
		m_Tex2DDesc.SampleDesc.Count = m_uMultiSamples;
		m_Tex2DDesc.SampleDesc.Quality = m_uMsQuality;

		if (m_eSurfaceType == SurfaceType::RenderTarget)
		{
			m_Tex2DDesc.BindFlags |= (uint32_t)BindFlags::RenderTarget;

			//if(createUAV)
			//	m_Tex2DDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
			
			//Se debería preguntar por la generación automática??
			if (m_uLevelCount > 1)
				m_Tex2DDesc.MiscFlags |= (uint32_t)ResourceOptionFlags::GenerateMipMaps;
		}

		if (m_bShared)
		{
			m_Tex2DDesc.MiscFlags |= (uint32_t)ResourceOptionFlags::Shared;
		}
		
		ID3D11Texture2D *localTexture = nullptr;
		DX::ThrowIfFailed(m_pDevice->GetD3DDevice()->CreateTexture2D(&m_Tex2DDesc, subdata, &localTexture));
		
		m_pTexture = localTexture;

		DX::ThrowIfFailed(m_pDevice->GetD3DDevice()->CreateShaderResourceView(m_pTexture, nullptr, &m_pShaderResourceView));
	}
#endif

	Texture2D::~Texture2D()
	{
		Texture::~Texture();
	}
}