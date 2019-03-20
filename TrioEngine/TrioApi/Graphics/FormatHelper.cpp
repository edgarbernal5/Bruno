#include "stdafx.h"
#include "FormatHelper.h"


namespace Vago
{
	//TO-DO: Faltan formatos...
	DXGI_FORMAT ToFormat(SurfaceFormat format)
	{
		switch (format)
		{
		case SurfaceFormat::Color:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case SurfaceFormat::Bgr565:
			return DXGI_FORMAT_B5G6R5_UNORM;
		case SurfaceFormat::Bgra5551:
			return DXGI_FORMAT_B5G5R5A1_UNORM;
		case SurfaceFormat::Bgra4444:
			return DXGI_FORMAT_B4G4R4A4_UNORM;
		case SurfaceFormat::Dxt1:
			return DXGI_FORMAT_BC1_UNORM;
		case SurfaceFormat::Dxt3:
			return DXGI_FORMAT_BC2_UNORM;
		case SurfaceFormat::Dxt5:
			return DXGI_FORMAT_BC3_UNORM;
		case SurfaceFormat::NormalizedByte2:
			return DXGI_FORMAT_R8G8_SNORM;
		case SurfaceFormat::NormalizedByte4:
			return DXGI_FORMAT_R8G8B8A8_SNORM;
		case SurfaceFormat::Alpha8:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case SurfaceFormat::Rgba1010102:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
		case SurfaceFormat::Rg32:
			return DXGI_FORMAT_R16G16_UNORM;
		case SurfaceFormat::Rgba64:
			return DXGI_FORMAT_R16G16B16A16_UNORM;
		case SurfaceFormat::Single:
			return DXGI_FORMAT_R32_FLOAT;
		case SurfaceFormat::HalfSingle:
			return DXGI_FORMAT_R16_FLOAT;
		case SurfaceFormat::HalfVector2:
			return DXGI_FORMAT_R16G16_FLOAT;
		case SurfaceFormat::Vector2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case SurfaceFormat::Vector4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SurfaceFormat::HalfVector4:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case SurfaceFormat::HdrBlendable:
			// TODO: This needs to check the graphics device and 
			// return the best hdr blendable format for the device.
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case SurfaceFormat::Bgr32:
			return DXGI_FORMAT_B8G8R8X8_UNORM;

		case SurfaceFormat::Bgra32:
			return DXGI_FORMAT_B8G8R8A8_UNORM;

		case SurfaceFormat::ColorSRgb:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case SurfaceFormat::Bgr32SRgb:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		case SurfaceFormat::Bgra32SRgb:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case SurfaceFormat::Dxt1SRgb:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case SurfaceFormat::Dxt3SRgb:
			return DXGI_FORMAT_BC2_UNORM_SRGB;

		case SurfaceFormat::Dxt5SRgb:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

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
		return format == DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT ||
			format == DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	}

	bool HasStencil(DepthFormat format)
	{
		return format == DepthFormat::Depth24Stencil8;
	}

	D3D_PRIMITIVE_TOPOLOGY FormatToPrimitive(PrimitiveType format)
	{
		switch (format)
		{
		case PrimitiveType::TriangleList:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case PrimitiveType::TriangleStrip:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case PrimitiveType::LineList:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case PrimitiveType::LineStrip:
			return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PrimitiveType::PointList:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		default:
			return (D3D_PRIMITIVE_TOPOLOGY)format;
		}
	}
	int GetElementCountArray(PrimitiveType format, int primitiveCount)
	{
		switch (format)
		{
		case PrimitiveType::TriangleList:
			return 3 * primitiveCount;
		case PrimitiveType::TriangleStrip:
			return 3 + (primitiveCount - 1); // ???
		case PrimitiveType::LineList:
			return primitiveCount * 2;
		case PrimitiveType::LineStrip:
			return primitiveCount + 1;
		case PrimitiveType::PointList:
			return primitiveCount;
		default:
			return 0;
		}
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

	int GetTypeSize(SurfaceFormat format)
	{
		switch (format)
		{
		case SurfaceFormat::Dxt1:
		case SurfaceFormat::Dxt1SRgb:
		case SurfaceFormat::Dxt1a:
		case SurfaceFormat::RgbPvrtc2Bpp:
		case SurfaceFormat::RgbaPvrtc2Bpp:
		case SurfaceFormat::RgbPvrtc4Bpp:
		case SurfaceFormat::RgbaPvrtc4Bpp:
		case SurfaceFormat::RgbEtc1:
			// One texel in DXT1, PVRTC (2bpp and 4bpp) and ETC1 is a minimum 4x4 block (8x4 for PVRTC 2bpp), which is 8 bytes
			return 8;
		case SurfaceFormat::Dxt3:
		case SurfaceFormat::Dxt3SRgb:
		case SurfaceFormat::Dxt5:
		case SurfaceFormat::Dxt5SRgb:
		case SurfaceFormat::RgbaAtcExplicitAlpha:
		case SurfaceFormat::RgbaAtcInterpolatedAlpha:
			//  One texel in DXT3 and DXT5 is a minimum 4x4 block, which is 16 bytes
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
		case SurfaceFormat::ColorSRgb:
		case SurfaceFormat::Single:
		case SurfaceFormat::Rg32:
		case SurfaceFormat::HalfVector2:
		case SurfaceFormat::NormalizedByte4:
		case SurfaceFormat::Rgba1010102:
		case SurfaceFormat::Bgra32:
		case SurfaceFormat::Bgra32SRgb:
		case SurfaceFormat::Bgr32:
		case SurfaceFormat::Bgr32SRgb:
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
			return 16;

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
}