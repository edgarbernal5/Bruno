#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioEngine
{
	class IServiceBase;

	class BRUNO_API_EXPORT IServiceProvider
	{
	public:
		virtual IServiceBase* GetService(std::string csTypeName) = 0;
	};

}
