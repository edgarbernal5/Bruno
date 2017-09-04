#include "stdafx.h"
#include "FormatHelper.h"


namespace Cuado
{

	int GetTypeSize(SurfaceFormat format)
	{
		switch (format)
		{
		case SurfaceFormat::Dxt1:
		case SurfaceFormat::Dxt1a:
		case SurfaceFormat::RgbPvrtc2Bpp:
		case SurfaceFormat::RgbaPvrtc2Bpp:
		case SurfaceFormat::RgbEtc1:
			// One texel in DXT1, PVRTC 2bpp and ETC1 is a minimum 4x4 block, which is 8 bytes
			return 8;
		case SurfaceFormat::Dxt3:
		case SurfaceFormat::Dxt5:
		case SurfaceFormat::RgbPvrtc4Bpp:
		case SurfaceFormat::RgbaPvrtc4Bpp:
			// One texel in DXT3, DXT5 and PVRTC 4bpp is a minimum 4x4 block, which is 16 bytes
			return 16;
		case SurfaceFormat::Alpha8:
			return 1;
		case SurfaceFormat::Bgr565:
		case SurfaceFormat::Bgra4444:
		case SurfaceFormat::Bgra5551:
		case SurfaceFormat::HalfSingle:
		case SurfaceFormat::NormalizedByte2:
			return 2;
		case SurfaceFormat::Color:
		case SurfaceFormat::Single:
		case SurfaceFormat::Rg32:
		case SurfaceFormat::HalfVector2:
		case SurfaceFormat::NormalizedByte4:
		case SurfaceFormat::Rgba1010102:
			return 4;
		case SurfaceFormat::HalfVector4:
		case SurfaceFormat::Rgba64:
		case SurfaceFormat::Vector2:
			return 8;
		case SurfaceFormat::Vector4:
			return 16;
			return 0;
		}
	}

	int GetTypeSize(VertexElementFormat elementFormat)
	{
		switch (elementFormat)
		{
		case VertexElementFormat::Single:
			return 4;

		case VertexElementFormat::Vector2:
			return 8;

		case VertexElementFormat::Vector3:
			return 12;

		case VertexElementFormat::Vector4:
			return 16;

		case VertexElementFormat::Color:
			return 4;

		case VertexElementFormat::Byte4:
			return 4;

		case VertexElementFormat::Short2:
			return 4;

		case VertexElementFormat::Short4:
			return 8;

		case VertexElementFormat::NormalizedShort2:
			return 4;

		case VertexElementFormat::NormalizedShort4:
			return 8;

		case VertexElementFormat::HalfVector2:
			return 4;

		case VertexElementFormat::HalfVector4:
			return 8;
		}
		return 0;
	}

	uint32_t GetFrameLatency(PresentInterval interval)
	{
		switch (interval)
		{
		case PresentInterval::Immediate:
			return 0;

		case PresentInterval::Two:
			return 2;

		default:
			return 1;
		}
	}


#if TRIO_DIRECTX


	//TO-DO: Faltan formatos...
	DXGI_FORMAT ToFormat(SurfaceFormat format)
	{
		switch (format)
		{
		case SurfaceFormat::Color:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case SurfaceFormat::Alpha8:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case SurfaceFormat::HalfSingle:
			return DXGI_FORMAT_R16_FLOAT;
		case SurfaceFormat::Single:
			return DXGI_FORMAT_R32_FLOAT;
		default:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
	}


	SurfaceFormat ToSurfaceFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return SurfaceFormat::Color;
		case DXGI_FORMAT_R16_FLOAT:
			return SurfaceFormat::HalfSingle;
		case DXGI_FORMAT_R32_FLOAT:
			return SurfaceFormat::Single;
		default:
			return SurfaceFormat::Color;
		}
	}

	//TO-DO: Faltan formatos...
	DXGI_FORMAT ToFormat(DepthFormat format)
	{
		switch (format)
		{
		case DepthFormat::Depth32:
			return DXGI_FORMAT_D32_FLOAT;
		default:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		}
	}

	DepthFormat FromFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_D32_FLOAT:
			return DepthFormat::Depth32;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return DepthFormat::Depth24Stencil8;
		default:
			return DepthFormat::Depth24Stencil8;
		}
	}

	bool HasStencil(DXGI_FORMAT format)
	{
		return format == DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT || format == DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	}

	bool HasStencil(DepthFormat format)
	{
		return format == DepthFormat::Depth24Stencil8;
	}



	DXGI_FORMAT ToFormat(IndexElementSize size)
	{
		switch (size)
		{
		case IndexElementSize::SixteenBits:
			return DXGI_FORMAT_R16_UINT;
		case IndexElementSize::ThirtyTwoBits:
			return DXGI_FORMAT_R32_UINT;
		default:
			return DXGI_FORMAT_R32_UINT;
		}
	}

	int GetTypeSize(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32_FLOAT:
			return 4;
		case DXGI_FORMAT_R32G32_FLOAT:
			return 8;
		case DXGI_FORMAT_R32G32B32_FLOAT:
			return 12;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return 16;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return 4;
		case DXGI_FORMAT_R8G8B8A8_UINT:
			return 4;
		case DXGI_FORMAT_R16G16_SINT:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return 8;
		case DXGI_FORMAT_R16G16_SNORM:
			return 8;
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			return 8;
		case DXGI_FORMAT_R16G16_FLOAT:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return 8;
		default:
			return 0;
		}
	}

	DXGI_SWAP_EFFECT ToSwapEffect(PresentInterval presentInterval)
	{
		DXGI_SWAP_EFFECT effect = DXGI_SWAP_EFFECT_DISCARD;

		switch (presentInterval)
		{
		case PresentInterval::Immediate:
			effect = DXGI_SWAP_EFFECT_SEQUENTIAL;
			break;
		case PresentInterval::One:
		case PresentInterval::Two:
		default:
			effect = DXGI_SWAP_EFFECT_DISCARD;
			break;
		}
		return effect;
	}
#endif
}