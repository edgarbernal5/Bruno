#pragma once

#include "TrioApiDecl.h"

#include "IServiceBase.h"
#include "Event.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class TRIOAPI_DECL IGraphicsDeviceService
	{
	public:
		virtual GraphicsDevice* GetGraphicsDevice() = 0;

		Event<> DeviceCreated;
	};

}
