#pragma once

#include "TrioAPI.h"

#include "IServiceBase.h"
namespace Cuado
{
	class TRIOAPI_DLL IGraphicsDeviceManager : public IServiceBase
	{
	public:
		virtual ~IGraphicsDeviceManager() {}
		virtual bool BeginDraw() = 0;
		virtual void CreateDevice() = 0;
		virtual void EndDraw() = 0;
	};
}