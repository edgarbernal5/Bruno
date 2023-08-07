#include "brpch.h"
#include "ModelReader.h"

#include "Bruno/Content/ContentReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelReader);

	ModelReader::ModelReader() :
		ContentTypeReader(Texture::GetTypeIdClass())
	{
	}

	std::shared_ptr<Texture> ModelReader::ReadInternal(ContentReader& input)
	{
		return nullptr;
	}
}