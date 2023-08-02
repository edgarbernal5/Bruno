#include "brpch.h"
#include "ShaderReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderReader);

	ShaderReader::ShaderReader() :
		ContentTypeReader(Shader::GetTypeIdClass())
	{
	}

	std::shared_ptr<Shader> ShaderReader::ReadInternal(const std::string& assetName)
	{
		return std::shared_ptr<Shader>();
	}
}
