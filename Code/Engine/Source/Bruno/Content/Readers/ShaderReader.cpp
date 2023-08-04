#include "brpch.h"
#include "ShaderReader.h"

#include "Bruno/Content/ContentReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderReader);

	ShaderReader::ShaderReader() :
		ContentTypeReader(Shader::GetTypeIdClass())
	{
	}

	std::shared_ptr<Shader> ShaderReader::ReadInternal(ContentReader& input)
	{
		std::vector<uint8_t> data;
		input.ReadBytes(data);
	
		return std::make_shared<Shader>(data);
	}
}
