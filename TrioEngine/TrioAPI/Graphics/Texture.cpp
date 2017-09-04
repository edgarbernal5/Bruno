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

#if TRIO_OPENGL
	void Texture::GetGLFormat(SurfaceFormat format, GLint& glinternalFormat, GLenum& glformat, GLenum& gltype)
	{
		glinternalFormat = GL_RGBA;
		glformat = GL_RGBA;
		gltype = GL_UNSIGNED_BYTE;

		switch (format)
		{
		case Cuado::SurfaceFormat::Color:
			glinternalFormat = GL_RGBA;
			glformat = GL_RGBA;
			gltype = GL_UNSIGNED_BYTE;

			break;
		case Cuado::SurfaceFormat::Bgr565:
			glinternalFormat = GL_RGB;
			glformat = GL_RGB;
			gltype = GL_UNSIGNED_SHORT_5_6_5;

			break;
		case Cuado::SurfaceFormat::Bgra5551:
			break;
		case Cuado::SurfaceFormat::Bgra4444:
			break;
		case Cuado::SurfaceFormat::Dxt1:
			break;
		case Cuado::SurfaceFormat::Dxt3:
			break;
		case Cuado::SurfaceFormat::Dxt5:
			break;
		case Cuado::SurfaceFormat::NormalizedByte2:
			break;
		case Cuado::SurfaceFormat::NormalizedByte4:
			break;
		case Cuado::SurfaceFormat::Rgba1010102:
			break;
		case Cuado::SurfaceFormat::Rg32:
			break;
		case Cuado::SurfaceFormat::Rgba64:
			break;
		case Cuado::SurfaceFormat::Alpha8:
			glinternalFormat = GL_LUMINANCE;
			glformat = GL_LUMINANCE;
			gltype = GL_UNSIGNED_BYTE;

			break;
		case Cuado::SurfaceFormat::Single:
			glinternalFormat = GL_R32F;
			glformat = GL_RED;
			gltype = GL_FLOAT;

			break;
		case Cuado::SurfaceFormat::Vector2:
			break;
		case Cuado::SurfaceFormat::Vector4:
			break;
		case Cuado::SurfaceFormat::HalfSingle:
			break;
		case Cuado::SurfaceFormat::HalfVector2:
			break;
		case Cuado::SurfaceFormat::HalfVector4:
			break;
		case Cuado::SurfaceFormat::HdrBlendable:
			break;
		case Cuado::SurfaceFormat::Bgr32:
			break;
		case Cuado::SurfaceFormat::Bgra32:
			break;
		case Cuado::SurfaceFormat::RgbPvrtc2Bpp:
			break;
		case Cuado::SurfaceFormat::RgbPvrtc4Bpp:
			break;
		case Cuado::SurfaceFormat::RgbaPvrtc2Bpp:
			break;
		case Cuado::SurfaceFormat::RgbaPvrtc4Bpp:
			break;
		case Cuado::SurfaceFormat::RgbEtc1:
			break;
		case Cuado::SurfaceFormat::Dxt1a:
			break;
		default:
			break;
		}
	}
#endif
}
