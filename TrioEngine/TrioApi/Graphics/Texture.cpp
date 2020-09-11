#include "stdafx.h"
#include "Texture.h"

namespace TrioEngine
{
	Texture::Texture() :
#ifdef TRIO_DIRECTX
		m_shaderResourceView(nullptr),
		m_texture(nullptr),
#endif
		m_device(nullptr),
		m_arraySize(0),
		m_levelCount(0)
	{
	}

	Texture::~Texture()
	{
#ifdef TRIO_DIRECTX
		RELEASE_COM(m_texture);
		RELEASE_COM(m_shaderResourceView);
#endif
	}

	Texture::Texture(GraphicsDevice* device, SurfaceFormat format) :
#ifdef TRIO_DIRECTX
		m_shaderResourceView(nullptr),
		m_texture(nullptr),
#elif TRIO_OPENGL
		m_glTexture(-1),
#endif
		m_device(device),
		m_format(format),
		m_arraySize(0),
		m_levelCount(0)
	{

	}

	int Texture::GetPitch(int width)
	{
		int pitch = 0;
		switch (m_format)
		{
		case SurfaceFormat::Dxt1:
		case SurfaceFormat::Dxt1SRgb:
		case SurfaceFormat::Dxt1a:
		case SurfaceFormat::RgbPvrtc2Bpp:
		case SurfaceFormat::RgbaPvrtc2Bpp:
		case SurfaceFormat::RgbEtc1:
		case SurfaceFormat::Dxt3:
		case SurfaceFormat::Dxt3SRgb:
		case SurfaceFormat::Dxt5:
		case SurfaceFormat::Dxt5SRgb:
		case SurfaceFormat::RgbPvrtc4Bpp:
		case SurfaceFormat::RgbaPvrtc4Bpp:
			//Debug.Assert(MathHelper.IsPowerOfTwo(width), "This format must be power of two!");
			pitch = ((width + 3) / 4) * FormatHelper::GetTypeSize(m_format);
			break;

		default:
			pitch = width * FormatHelper::GetTypeSize(m_format);
			break;
		};

		return pitch;
	}
}