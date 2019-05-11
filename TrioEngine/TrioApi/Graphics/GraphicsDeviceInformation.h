#pragma once

#include "TrioApiDecl.h"

#include "PresentationParameters.h"

namespace TrioEngine
{
	class GraphicsAdapter;
	class GraphicsDeviceManager;

	struct TRIOAPI_DECL GraphicsDeviceInformation
	{
		GraphicsAdapter* Adapter;
		PresentationParameters Parameters;

		bool operator ==(const GraphicsDeviceInformation &other) const
		{
			return Parameters == other.Parameters && Adapter == other.Adapter;
		}
	};

	
}