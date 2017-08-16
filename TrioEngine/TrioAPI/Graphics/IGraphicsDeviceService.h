#pragma once

#include "TrioAPI.h"

#include "IServiceBase.h"
#include "Event.h"

namespace Cuado
{

	class TRIOAPI_DLL GraphicsDevice;
	class TRIOAPI_DLL IGraphicsDeviceService : public IServiceBase
	{
	public:
		virtual GraphicsDevice* GetGraphicsDevice() = 0;

		Event<> DeviceCreated;
	};
}