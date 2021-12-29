#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	class GraphicsDevice;
	class ContentReader;

	class BRUNO_API_EXPORT GraphicsContentHelper
	{
	public:
		static GraphicsDevice* GraphicsDeviceFromContentReader(ContentReader* contentReader);
	};
}