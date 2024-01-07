#pragma once

#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	struct ContentItem : public RTTI
	{
		BR_RTTI_DECLARATION(ContentItem, RTTI);

	public:
		virtual ~ContentItem() = default;
	};
}