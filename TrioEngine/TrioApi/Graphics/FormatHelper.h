#pragma once

#include "DepthFormat.h"
#include "IndexElementSize.h"
#include "PresentInterval.h"
#include "PrimitiveType.h"
#include "SurfaceFormat.h"
#include "VertexElementFormat.h"

namespace TrioEngine
{
	class FormatHelper {
	public:
		static DXGI_FORMAT ToFormat(SurfaceFormat format);
		static  DXGI_FORMAT ToFormat(DepthFormat format);
		static DepthFormat FromFormat(DXGI_FORMAT format);
		static SurfaceFormat ToSurfaceFormat(DXGI_FORMAT format);
		static bool HasStencil(DXGI_FORMAT format);
		static bool HasStencil(DepthFormat format);

		static D3D_PRIMITIVE_TOPOLOGY FormatToPrimitive(PrimitiveType format);
		static int GetElementCountArray(PrimitiveType format, int primitiveCount);
		static DXGI_FORMAT ToFormat(IndexElementSize size);

		static int GetByteSizeFrom(DXGI_FORMAT format);
		static int GetByteSizeFrom(SurfaceFormat format);
		static int GetByteSizeFrom(VertexElementFormat elementFormat);

		static DXGI_SWAP_EFFECT ToSwapEffect(PresentInterval presentInterval);
		static uint32_t GetFrameLatency(PresentInterval interval);
	};
}