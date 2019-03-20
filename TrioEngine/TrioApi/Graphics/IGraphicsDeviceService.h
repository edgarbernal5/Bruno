#pragma once

#include "TrioApiDecl.h"

#include "IServiceBase.h"
#include "Event.h"

namespace Vago
{
	class GraphicsDevice;

	class TRIOAPI_DECL IGraphicsDeviceService
	{
	public:
		virtual GraphicsDevice* GetGraphicsDevice() = 0;

		Event<> DeviceCreated;
	};

}
