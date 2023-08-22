#include "brpch.h"
#include "Shader.h"

#include "ShaderProgram.h"
#include "Bruno/Core/FileStream.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Shader);

	const Shader::ShaderTypeDesc& Shader::VS = { ShaderProgramType::Vertex, "main_VS", "vs_5_1", D3D12_SHADER_VISIBILITY_VERTEX};
	const Shader::ShaderTypeDesc& Shader::PS = { ShaderProgramType::Pixel, "main_PS", "ps_5_1", D3D12_SHADER_VISIBILITY_PIXEL};
	
	const Shader::ShaderTypeDesc Shader::ShaderTypes[] = {Shader::VS, Shader::PS};

	Shader::Shader(std::vector<std::vector<uint8_t>>&& programsData)
	{
		for (size_t i = 0; i < programsData.size(); i++)
		{
			if (programsData[i].size() > 0) {
				m_programs[i] = std::make_shared<ShaderProgram>(std::move(programsData[i]));
			}
		}
	}

	Shader::Shader(const std::wstring& sourceFilename)
	{
		FileStream fileStream(sourceFilename, FileAccess::Read);
		if (!fileStream.IsStreamValid())
			return;

		std::string content;
		std::vector<uint8_t> rawBytes;
		fileStream.ReadBytes(rawBytes, fileStream.GetLength());
		content = std::string(rawBytes.data(), rawBytes.data() + rawBytes.size());

		for (size_t i = 0; i < 2; i++)
		{
			auto entryPointIndex = content.find_first_of(ShaderTypes[i].EntryPoint);
			if (entryPointIndex != std::string::npos) {
				m_programs[i] = std::make_shared<ShaderProgram>(sourceFilename, ShaderTypes[i].EntryPoint, ShaderTypes[i].Target);
			}
		}
	}
}