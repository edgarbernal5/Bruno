#pragma once

#include "DeviceFormats.h"
#include "ResourceEnums.h"

#include "VertexElement.h"
#include "StatesEnums.h"

namespace Cuado
{
	enum class VertexElementFormat;

	DXGI_FORMAT ToFormat(SurfaceFormat format);
	DXGI_FORMAT ToFormat(DepthFormat format);
	DepthFormat FromFormat(DXGI_FORMAT format);
	SurfaceFormat ToSurfaceFormat(DXGI_FORMAT format);
	bool HasStencil(DXGI_FORMAT format);
	bool HasStencil(DepthFormat format);

	//D3D_PRIMITIVE_TOPOLOGY FormatToPrimitive(PrimitiveType format);
	//int GetElementCountArray(PrimitiveType format, int primitiveCount);
	DXGI_FORMAT ToFormat(IndexElementSize size);

	int GetTypeSize(DXGI_FORMAT format);
	int GetTypeSize(SurfaceFormat format);
	int GetTypeSize(VertexElementFormat elementFormat);

	DXGI_SWAP_EFFECT ToSwapEffect(PresentInterval presentInterval);
	uint32_t GetFrameLatency(PresentInterval interval);
}