#pragma once

#include "Bruno/Core/RTTI.h"
#include <string>

namespace Bruno
{
	class Material : public RTTI
	{
		BR_RTTI_DECLARATION(Material, RTTI);

	public:

		std::string Name;
	};
}
