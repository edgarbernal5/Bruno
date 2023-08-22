#pragma once

#include <vector>
#include "Pipeline/ContentItem.h"

namespace Bruno
{
	struct ShaderContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(ShaderContentItem, ContentItem);
	
	public:
		
		std::vector<uint8_t> CompiledCodeProgramsData[2]{};
	};

}