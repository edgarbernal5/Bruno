#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	class GraphicsDevice;
	class ContentReader;

	class TRIO_API_EXPORT GraphicsContentHelper
	{
	public:
		static GraphicsDevice* GraphicsDeviceFromContentReader(ContentReader* contentReader);
	};
}