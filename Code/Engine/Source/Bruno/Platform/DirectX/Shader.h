#pragma once

#include <d3d12.h>
#include "d3dx12.h"
#include "D3DConstants.h"

#include "PipelineStateObject.h"
#include "Bruno/Core/RTTI.h"
#include "Bruno/Content/Asset.h"
#include <vector>
#include <string>
#include <map>

namespace Bruno
{
	class ShaderProgram;
	class RootSignature;

	class Shader : public Asset
	{
		BR_RTTI_DECLARATION(Shader, Asset);

	public:
		enum class ShaderProgramType : uint8_t
		{
			Vertex = 0,
			Pixel
		};

		struct Sampler
		{
			std::string Name;
			uint32_t BindPoint, BindSlot;
			D3D12_SHADER_VISIBILITY Visibility;
			D3D12_STATIC_SAMPLER_DESC Desc;

			void Setup();
		};

		Shader(std::array<std::vector<uint8_t>, Graphics::Core::SHADER_PROGRAMS_COUNT>&& programsData);
		Shader(const std::wstring& sourceFilename);
		
		D3D12_INPUT_LAYOUT_DESC GetInputLayout();
		std::shared_ptr<RootSignature> CreateRootSignature(PipelineResourceMapping& resourceMapping);

		ShaderProgram* GetShaderProgram(ShaderProgramType type);
		uint32_t GetIndexMap(const std::wstring& name);
	private:
		struct ShaderTypeDesc
		{
			ShaderProgramType Type;
			const char* EntryPoint;
			const char* Target;
			D3D12_SHADER_VISIBILITY Visibility;
		};

		void InitializeParameters();
		void InitializeParameters(ShaderProgram* program);

		std::array<std::shared_ptr<ShaderProgram>, Graphics::Core::SHADER_PROGRAMS_COUNT> m_programs;
		std::shared_ptr<RootSignature> m_rootSignature;
		std::map<std::wstring, uint32_t> m_rootParameterIndexMap;

		std::vector<CD3DX12_ROOT_PARAMETER1> m_rootParameters;
		std::vector<CD3DX12_DESCRIPTOR_RANGE1> m_descriptorRanges;
		std::vector<Sampler> m_samplers;

		static const ShaderTypeDesc& VS, & PS;
		static const ShaderTypeDesc ShaderTypes[];
	};
}

