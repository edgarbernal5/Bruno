#include "stdafx.h"
#include "Texture.h"

#include "FormatHelper.h"

namespace Cuado
{
	Texture::Texture()
	{
	}


	Texture::~Texture()
	{
	}

	Texture::Texture(GraphicsDevice* device, SurfaceFormat format) :
#ifdef TRIO_DIRECTX
		m_pShaderResourceView(nullptr),
		m_pTexture(nullptr),
#elif TRIO_OPENGL
		m_glTexture(-1),
#endif
		m_pDevice(device),
		m_eFormat(format)
	{


	}


	int Texture::GetPitch(int width)
	{
		int pitch;

		switch (m_eFormat)
		{
		case SurfaceFormat::Dxt1:
		case SurfaceFormat::Dxt1a:
		case SurfaceFormat::RgbPvrtc2Bpp:
		case SurfaceFormat::RgbaPvrtc2Bpp:
		case SurfaceFormat::RgbEtc1:
		case SurfaceFormat::Dxt3:
		case SurfaceFormat::Dxt5:
		case SurfaceFormat::RgbPvrtc4Bpp:
		case SurfaceFormat::RgbaPvrtc4Bpp:
			//Debug.Assert(MathHelper.IsPowerOfTwo(width), "This format must be power of two!");
			pitch = ((width + 3) / 4) * GetTypeSize(m_eFormat);
			break;

		default:
			pitch = width * GetTypeSize(m_eFormat);
			break;
		};

		return pitch;
	}
}
