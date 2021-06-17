#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	enum class ResourceOptionFlags
	{
		None = 0,
		GenerateMipMaps = 1,
		Shared = 2,
		TextureCube = 4,
		DrawIndirectArguments = 16,
		BufferAllowRawViews = 32,
		BufferStructured = 64,
		ResourceClamp = 128,
		SharedKeyedmutex = 256,
		GdiCompatible = 512,
		SharedNthandle = 2048,
		RestrictedContent = 4096,
		RestrictSharedResource = 8192,
		RestrictSharedResourceDriver = 16384,
		Guarded = 32768,
		TilePool = 131072,
		Tiled = 262144,
		HwProtected = 524288
	};
	DEFINE_ENUM_FLAG_OPERATORS(ResourceOptionFlags);
}
