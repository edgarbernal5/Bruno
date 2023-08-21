#pragma once

#include "D3DCommon.h"
#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	enum class ShaderType : uint32_t
	{
		Vertex = 0,
		Pixel,

		Count
	};

	class Shader : public RTTI
	{
		BR_RTTI_DECLARATION(Shader, RTTI);

	public:
		Shader(std::vector<uint8_t>& data);
		Shader(const std::wstring& sourceFilename);

		void SetShaderProgram(const std::wstring& sourceFilename, std::string entryPoint, std::string target);
	private:
		//std::shared_ptr<ShaderProgram> m_programs[3]{};
	};
}

