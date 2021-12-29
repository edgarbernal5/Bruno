#pragma once

#include "TrioApiRequisites.h"

#include "IServiceBase.h"
#include "Event.h"

namespace BrunoEngine
{
	class GraphicsDevice;

	class BRUNO_API_EXPORT IGraphicsDeviceService
	{
	public:
		virtual GraphicsDevice* GetGraphicsDevice() = 0;

		Event<> DeviceCreated;
	};

}
