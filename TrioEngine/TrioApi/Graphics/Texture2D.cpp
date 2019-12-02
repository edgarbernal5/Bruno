#include "stdafx.h"
#include "Texture2D.h"

#include "FormatHelper.h"
#include "BindFlags.h"
#include "ResourceOptionFlags.h"
#include "CpuAccessFlags.h"

namespace TrioEngine
{

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, SurfaceFormat format) :
		Texture(graphicsDevice, format)
	{
		CommonConstructor(width, height, 1, SurfaceType::TextureType, 1, 0, false, 1);
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format) :
		Texture(graphicsDevice, format)
	{
		CommonConstructor(width, height, mipmap, SurfaceType::TextureType, 1, 0, false, 1);
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height) :
		Texture(graphicsDevice, SurfaceFormat::Color)
	{
		CommonConstructor(width, height, 1, SurfaceType::TextureType, 1, 0, false, 1);
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format, SurfaceType type, uint32_t multiSamples, uint32_t msQuality, bool shared) :
		Texture(graphicsDevice, format),
		m_width(width),
		m_height(height),
		m_surfaceType(type),
		m_shared(shared)
	{
		CommonConstructor(width, height, mipmap, type, multiSamples, msQuality, shared, 1);
	}

	void Texture2D::CommonConstructor(int width, int height, uint32_t mipmap, SurfaceType type, uint32_t multiSamples, uint32_t msQuality, bool shared, uint32_t uArraySize)
	{
		m_width = width;
		m_height = height;
		m_surfaceType = type;
		m_shared = shared;

		m_levelCount = mipmap;
		m_multiSamples = multiSamples;
		m_msQuality = msQuality;

		m_arraySize = uArraySize;

		m_usage = ResourceUsage::Default;

		if (type == SurfaceType::SwapChainRenderTarget)
			return;

		//
#ifdef TRIO_DIRECTX
		
#elif TRIO_OPENGL
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

			glTexParameteri(m_glTarget, GL_TEXTURE_MIN_FILTER, (m_LevelCount > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(m_glTarget, GL_TEXTURE_MAG_FILTER, (m_LevelCount > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_T, wrap);
		}

		GetGLFormat(m_Format, m_glInternalFormat, m_glFormat, m_glType);
		//TODO:
		//OJO: el orden de los factores si altera el producto (colocar la siguiente linea luego de bindear)
		glTexImage2D(GL_TEXTURE_2D, 0, m_glInternalFormat, m_Width, m_Height, 0, m_glFormat, m_glType, nullptr);

		glBindTexture(m_glTarget, prevTexture);
#endif
	}

#ifdef TRIO_DIRECTX
	void Texture2D::CreateTexture(D3D11_SUBRESOURCE_DATA * subdata)
	{
		m_tex2DDesc = { 0 };
		m_tex2DDesc.Width = m_width;
		m_tex2DDesc.Height = m_height;
		m_tex2DDesc.MipLevels = m_levelCount;
		m_tex2DDesc.ArraySize = m_arraySize;
		m_tex2DDesc.Format = ToFormat(m_format);
		m_tex2DDesc.BindFlags = (UINT)BindFlags::ShaderResource;
		m_tex2DDesc.CPUAccessFlags = (UINT)CpuAccessFlags::None;
		m_tex2DDesc.SampleDesc.Count = m_multiSamples;
		m_tex2DDesc.SampleDesc.Quality = m_msQuality;
		m_tex2DDesc.Usage = (D3D11_USAGE)m_usage;
		m_tex2DDesc.MiscFlags = (UINT)ResourceOptionFlags::None;

		if (m_surfaceType == SurfaceType::RenderTarget)
		{
			m_tex2DDesc.BindFlags |= (uint32_t)BindFlags::RenderTarget;

			//if(createUAV)
			//	m_Tex2DDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

			//Se debería preguntar por la generación automática??
			if (m_levelCount > 1)
				m_tex2DDesc.MiscFlags |= (uint32_t)ResourceOptionFlags::GenerateMipMaps;
		}

		if (m_shared)
		{
			m_tex2DDesc.MiscFlags |= (uint32_t)ResourceOptionFlags::Shared;
		}

		ID3D11Texture2D *localTexture = nullptr;
		DX::ThrowIfFailed(m_device->GetD3DDevice()->CreateTexture2D(&m_tex2DDesc, subdata, &localTexture));

		m_texture = localTexture;

		DX::ThrowIfFailed(m_device->GetD3DDevice()->CreateShaderResourceView(m_texture, nullptr, &m_shaderResourceView));
	}
#endif

	Texture2D::~Texture2D()
	{
		Texture::~Texture();
	}
}