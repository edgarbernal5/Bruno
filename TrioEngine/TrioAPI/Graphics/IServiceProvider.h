#pragma once

#include "TrioAPI.h"
#include "IServiceBase.h"

#include <string>

namespace Cuado
{
	class TRIOAPI_DLL IServicesProvider
	{
	public:
		virtual IServiceBase* GetService(std::string typeName) = 0;
	};
}