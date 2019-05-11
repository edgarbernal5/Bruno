#pragma once

#include "TrioApiDecl.h"

#include "IServiceBase.h"

namespace TrioEngine
{
	class TRIOAPI_DECL IGraphicsDeviceManager : public IServiceBase
	{
	public:
		virtual ~IGraphicsDeviceManager() {}
		virtual bool BeginDraw() = 0;
		virtual void CreateDevice() = 0;
		virtual void EndDraw() = 0;
	};

}
