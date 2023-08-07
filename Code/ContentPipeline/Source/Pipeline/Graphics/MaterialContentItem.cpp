#include "MaterialContentItem.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(MaterialContentItem);
	
	void MaterialContentItem::AddTexture(const std::string& key, const ExternalReferenceContentItem& reference)
	{
		m_textureByKeys[key] = reference;
	}
}
