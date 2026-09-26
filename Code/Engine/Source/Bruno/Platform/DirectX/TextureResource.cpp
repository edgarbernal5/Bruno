#include "brpch.h"
#include "TextureResource.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(TextureResource);

	TextureResource::TextureResource(uint32_t width, uint32_t height, TextureFormat format, uint32_t mipMapLevels) :
		m_width(width),
		m_height(height),
		m_mipMapLevels(mipMapLevels),
		m_format(format)
	{
	}

	TextureResource::~TextureResource() = default;
}
