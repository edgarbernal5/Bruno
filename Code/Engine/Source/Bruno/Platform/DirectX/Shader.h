#pragma once

#include "D3DCommon.h"
#include "Bruno/Core/RTTI.h"

namespace Bruno
{

	class ShaderProgram;

	class Shader : public RTTI
	{
		BR_RTTI_DECLARATION(Shader, RTTI);

	public:
		Shader(std::vector<std::vector<uint8_t>>&& programsData);
		Shader(const std::wstring& sourceFilename);
				
	private:
		enum class ShaderProgramType : uint32_t
		{
			Vertex = 0,
			Pixel
		};
		struct ShaderTypeDesc
		{
			ShaderProgramType Type;
			const char* EntryPoint;
			const char* Target;
			D3D12_SHADER_VISIBILITY Visibility;
		};
		std::shared_ptr<ShaderProgram> m_programs[2]{};

		static const ShaderTypeDesc& VS, & PS;
		static const ShaderTypeDesc ShaderTypes[];
	};
}

