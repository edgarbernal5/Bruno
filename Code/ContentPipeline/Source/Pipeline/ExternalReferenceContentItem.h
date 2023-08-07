#pragma once

#include <vector>
#include "Pipeline/ContentItem.h"

namespace Bruno
{
	struct ExternalReferenceContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(ExternalReferenceContentItem, ContentItem);

	public:
		ExternalReferenceContentItem() = default;
		ExternalReferenceContentItem(const std::wstring& filename);
		std::wstring Filename;
	};

}