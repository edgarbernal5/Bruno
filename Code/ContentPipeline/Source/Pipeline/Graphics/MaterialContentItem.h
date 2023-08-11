#pragma once

#include <vector>
#include <map>

#include "Pipeline/ContentItem.h"
#include "Pipeline/ExternalReferenceContentItem.h"

namespace Bruno
{
	struct MaterialContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(MaterialContentItem, ContentItem);

	public:
		std::string Name;
		std::map<std::string, ExternalReferenceContentItem> TexturesByKeys;

		void AddTexture(const std::string& key, const ExternalReferenceContentItem& reference);
	};

}