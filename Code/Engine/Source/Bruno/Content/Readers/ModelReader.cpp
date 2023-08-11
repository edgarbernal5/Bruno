#include "brpch.h"
#include "ModelReader.h"

#include "Bruno/Content/ContentReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelReader);

	ModelReader::ModelReader() :
		ContentTypeReader(Model::GetTypeIdClass())
	{
	}

	std::shared_ptr<Model> ModelReader::ReadInternal(ContentReader& input)
	{
		auto model = std::make_shared<Model>();
		return model;
	}
}