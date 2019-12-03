#pragma once

#include "TrioApiRequisites.h"

#include "IServiceBase.h"
#include "Event.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class TRIO_API_EXPORT IGraphicsDeviceService
	{
	public:
		virtual GraphicsDevice* GetGraphicsDevice() = 0;

		Event<> DeviceCreated;
	};

}
