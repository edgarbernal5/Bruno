#include "brpch.h"
#include "Shader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Shader);

	Shader::Shader(std::vector<uint8_t>& data)
	{
	}

	Shader::Shader(const std::wstring& sourceFilename)
	{
	}
	
	void Shader::SetShaderProgram(const std::wstring& sourceFilename, std::string entryPoint, std::string target)
	{
	}
}