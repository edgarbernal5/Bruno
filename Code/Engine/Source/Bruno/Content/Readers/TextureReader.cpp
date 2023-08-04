#include "brpch.h"
#include "TextureReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(TextureReader);

	TextureReader::TextureReader() :
		ContentTypeReader(Texture::GetTypeIdClass())
	{
	}

	std::shared_ptr<Texture> TextureReader::ReadInternal(ContentReader& input)
	{
		return std::shared_ptr<Texture>();
	}
}