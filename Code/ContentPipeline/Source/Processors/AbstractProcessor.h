#pragma once

#include <Bruno/Core/RTTI.h>

namespace Bruno
{
	class AbstractProcessor : public RTTI
	{
		BR_RTTI_DECLARATION(AbstractProcessor, RTTI);
	};
}