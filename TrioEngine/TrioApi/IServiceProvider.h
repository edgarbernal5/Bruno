#pragma once

#include "TrioApiDecl.h"

#include <string>

namespace Vago
{
	class IServiceBase;

	class TRIOAPI_DECL IServiceProvider
	{
	public:
		virtual IServiceBase* GetService(std::string csTypeName) = 0;
	};

}
