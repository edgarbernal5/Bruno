#include "brpch.h"
#include "MaterialReader.h"

#include "Bruno/Content/ContentReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(MaterialReader);

	MaterialReader::MaterialReader() :
		ContentTypeReader(Texture::GetTypeIdClass())
	{
	}

	std::shared_ptr<Texture> MaterialReader::ReadInternal(ContentReader& input)
	{
		return nullptr;
	}
}