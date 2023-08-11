#include "brpch.h"
#include "MaterialReader.h"

#include "Bruno/Content/ContentReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(MaterialReader);

	MaterialReader::MaterialReader() :
		ContentTypeReader(Material::GetTypeIdClass())
	{
	}

	std::shared_ptr<Material> MaterialReader::ReadInternal(ContentReader& input)
	{
		auto material = std::make_shared<Material>();
		input.ReadString(material->Name);

		return material;
	}
}