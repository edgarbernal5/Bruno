#include "stdafx.h"
#include "Texture2D.h"

#include "FormatHelper.h"
#include "BindFlags.h"
#include "ResourceOptionFlags.h"
#include "CpuAccessFlags.h"

namespace BrunoEngine
{

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, SurfaceFormat format) :
		Texture(graphicsDevice, format)
#if BRUNO_DIRECTX
		, m_stagingTex (nullptr)
#endif
	{
		CommonConstructor(width, height, 1, SurfaceType::TextureType, 1, 0, false, 1);
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format) :
		Texture(graphicsDevice, format)
#if BRUNO_DIRECTX
		, m_stagingTex(nullptr)
#endif
	{
		CommonConstructor(width, height, mipmap, SurfaceType::TextureType, 1, 0, false, 1);
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height) :
		Texture(graphicsDevice, SurfaceFormat::Color)
#if BRUNO_DIRECTX
		, m_stagingTex(nullptr)
#endif
	{
		CommonConstructor(width, height, 1, SurfaceType::TextureType, 1, 0, false, 1);
	}

	Texture2D::Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmapLevels, SurfaceFormat format, SurfaceType surfaceType, uint32_t multiSamples, uint32_t msQuality, bool shared) :
		Texture(graphicsDevice, format),
		m_width(width),
		m_height(height),
		m_surfaceType(surfaceType),
		m_shared(shared)
#if BRUNO_DIRECTX
		,
		m_stagingTex(nullptr)
#endif
	{
		CommonConstructor(width, height, mipmapLevels, surfaceType, multiSamples, msQuality, shared, 1);
	}

	void Texture2D::CommonConstructor(int width, int height, uint32_t mipmapLevels, SurfaceType surfaceType, uint32_t multiSamples, uint32_t msQuality, bool shared, uint32_t uArraySize)
	{
		m_width = width;
		m_height = height;
		m_surfaceType = surfaceType;
		m_shared = shared;

		m_levelCount = mipmapLevels == 0 ? 1 : mipmapLevels;
		m_multiSamples = multiSamples;
		m_msQuality = msQuality;

		m_arraySize = uArraySize;

		m_usage = ResourceUsage::Default;

		if (surfaceType == SurfaceType::SwapChainRenderTarget)
			return;

		//
#ifdef BRUNO_DIRECTX
		
#elif BRUNO_OPENGL
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

	Texture2D::~Texture2D()
	{
		RELEASE_COM(m_stagingTex);
		Texture::~Texture();
	}

#ifdef BRUNO_DIRECTX
	void Texture2D::CreateTexture(D3D11_SUBRESOURCE_DATA* subdata)
	{
		m_tex2DDesc = { 0 };
		m_tex2DDesc.Width = m_width;
		m_tex2DDesc.Height = m_height;
		m_tex2DDesc.MipLevels = m_levelCount;
		m_tex2DDesc.ArraySize = m_arraySize;
		m_tex2DDesc.Format = FormatHelper::ToFormat(m_format);
		m_tex2DDesc.BindFlags = (uint32_t)BindFlags::ShaderResource;
		m_tex2DDesc.CPUAccessFlags = (uint32_t)CpuAccessFlags::None;
		m_tex2DDesc.SampleDesc.Count = m_multiSamples;
		m_tex2DDesc.SampleDesc.Quality = m_msQuality;
		m_tex2DDesc.Usage = (D3D11_USAGE)m_usage;
		m_tex2DDesc.MiscFlags = (uint32_t)ResourceOptionFlags::None;

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

	void Texture2D::CreateStagingTexture()
	{
		if (m_stagingTex == nullptr)
		{
			D3D11_TEXTURE2D_DESC desc = { 0 };
			desc.Width = m_width;
			desc.Height = m_height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = FormatHelper::ToFormat(m_format);
			desc.BindFlags = (UINT)BindFlags::None;
			desc.CPUAccessFlags = (UINT)CpuAccessFlags::Read;
			desc.MiscFlags = (UINT)ResourceOptionFlags::None;
			desc.SampleDesc.Count = m_multiSamples;
			desc.SampleDesc.Quality = m_msQuality;
			desc.Usage = (D3D11_USAGE)ResourceUsage::Staging;

			//TO-DO: hacer un pool de texturas staging.
			DX::ThrowIfFailed(
				m_device->GetD3DDevice()->CreateTexture2D(&desc, nullptr, &m_stagingTex)
			);
		}
	}
#endif

	void Texture2D::GetData(int level, Rectangle* rect, uint8_t* data, int startIndex, int elementCount)
	{
		if (data == nullptr)
			return;

		int rectLeft, rectTop, rectWidth, rectHeight;
		if (rect != nullptr)
		{
			rectLeft = rect->x;
			rectTop = rect->y;
			rectWidth = rect->width;
			rectHeight = rect->height;
		}
		else
		{
			rectLeft = 0;
			rectTop = 0;
			rectWidth = std::max<int>(m_width >> level, 1);
			rectHeight = std::max<int>(m_height >> level, 1);

			//TO-DO: modificar h y w con formatos Dxt1, etc.
			// Ref: http://www.mentby.com/Group/mac-opengl/issue-with-dxt-mipmapped-textures.html 
			if (m_format == SurfaceFormat::Dxt1 ||
				m_format == SurfaceFormat::Dxt1a ||
				m_format == SurfaceFormat::Dxt3 ||
				m_format == SurfaceFormat::Dxt5)
			{
				rectWidth = (rectWidth + 3) & ~3;
				rectHeight = (rectHeight + 3) & ~3;
			}
		}

#ifdef BRUNO_DIRECTX
		CreateStagingTexture();

		D3D11_BOX region;
		region.front = 0;
		region.back = 1;

		region.top = rectTop;
		region.bottom = rectTop + rectHeight;
		region.left = rectLeft;
		region.right = rectLeft + rectWidth;

		//m_device->GetD3DDeviceContext()->CopySubresourceRegion1(m_stagingTex, 0, 0, 0, 0, m_texture, level, &region, D3D11_COPY_NO_OVERWRITE);
		m_device->GetD3DDeviceContext()->CopySubresourceRegion(m_stagingTex, 0, 0, 0, 0, m_texture, level, &region);

		int baseElementInBytes = FormatHelper::GetByteSizeFrom(m_format);
		int elementsInRow = rectWidth;
		int rowSizeInBytes = baseElementInBytes * elementsInRow;

		D3D11_MAPPED_SUBRESOURCE mapsource;
		m_device->GetD3DDeviceContext()->Map(m_stagingTex, 0, (D3D11_MAP)MapMode::Read, 0, &mapsource);

		uint8_t* pData = reinterpret_cast<uint8_t*>(mapsource.pData);

		int totalBytes = baseElementInBytes * elementCount;
		if (rowSizeInBytes == mapsource.RowPitch)
		{
			memcpy(data + startIndex, pData + startIndex, totalBytes);
		}
		else
		{
			int offsetpData = 0;
			for (size_t row = 0; row < rectHeight; row++)
			{
				int maxElements = (row + 1) * rowSizeInBytes;
				int rowStart = row * rowSizeInBytes;

				memcpy(data + rowStart + startIndex, pData + offsetpData + rowStart + startIndex, rowSizeInBytes);

				/*if (maxElements >= elementCount * tInBytes)
					break;*/

				offsetpData += mapsource.RowPitch - rowSizeInBytes;
			}
		}
		m_device->GetD3DDeviceContext()->Unmap(m_stagingTex, 0);
#endif
	}

}