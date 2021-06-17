#pragma once

#include "TrioApiRequisites.h"

#include "PresentationParameters.h"

namespace TrioEngine
{
	class GraphicsAdapter;
	class GraphicsDeviceManager;

	struct BRUNO_API_EXPORT GraphicsDeviceInformation
	{
		GraphicsAdapter* Adapter;
		PresentationParameters Parameters;

		bool operator ==(const GraphicsDeviceInformation &other) const
		{
			return Parameters == other.Parameters && Adapter == other.Adapter;
		}
	};

	
}