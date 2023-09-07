#include "brpch.h"
#include "Shader.h"

#include "ShaderProgram.h"
#include "RootSignature.h"
#include "Bruno/Core/FileStream.h"
#include "Bruno/Core/StringHelpers.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Shader);

	const Shader::ShaderTypeDesc& Shader::VS = { ShaderProgramType::Vertex, "main_VS", "vs_5_1", D3D12_SHADER_VISIBILITY_VERTEX};
	const Shader::ShaderTypeDesc& Shader::PS = { ShaderProgramType::Pixel, "main_PS", "ps_5_1", D3D12_SHADER_VISIBILITY_PIXEL};
	
	const Shader::ShaderTypeDesc Shader::ShaderTypes[] = {Shader::VS, Shader::PS};

	Shader::Shader(std::vector<std::vector<uint8_t>>&& programsData)
	{
		BR_ASSERT(programsData.size() <= Graphics::Core::SHADER_PROGRAMS_COUNT);
		
		for (size_t i = 0; i < programsData.size(); i++)
		{
			if (programsData[i].size() > 0)
			{
				m_programs[i] = std::make_shared<ShaderProgram>(std::move(programsData[i]), ShaderTypes[i].Visibility);
			}
		}
		InitializeParameters();
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

		for (size_t i = 0; i < Graphics::Core::SHADER_PROGRAMS_COUNT; i++)
		{
			auto entryPointIndex = content.find(ShaderTypes[i].EntryPoint);
			if (entryPointIndex != std::string::npos)
			{
				m_programs[i] = std::make_shared<ShaderProgram>(sourceFilename, ShaderTypes[i].EntryPoint, ShaderTypes[i].Target, ShaderTypes[i].Visibility);
			}
		}
		InitializeParameters();
	}

	D3D12_INPUT_LAYOUT_DESC Shader::GetInputLayout()
	{
		if (m_programs[0])
			return m_programs[0]->GetInputLayout();

		return D3D12_INPUT_LAYOUT_DESC();
	}

	std::shared_ptr<RootSignature> Shader::CreateRootSignature()
	{
		//m_rootSignature
		return std::shared_ptr<RootSignature>();
	}

	ShaderProgram* Shader::GetShaderProgram(ShaderProgramType type)
	{
		if (m_programs[(int)type])
			return m_programs[(int)type].get();
		return nullptr;
	}

	void Shader::InitializeParameters()
	{
		m_rootParameters.clear();
		m_descriptorRanges.clear();
		m_samplers.clear();

		for (size_t i = 0; i < Graphics::Core::SHADER_PROGRAMS_COUNT; i++)
		{
			if (m_programs[i])
				InitializeParameters(m_programs[i].get());
		}
	}

	void Shader::InitializeParameters(ShaderProgram* program)
	{
		ID3D12ShaderReflection* reflector = nullptr;
		D3DReflect(program->GetBlob()->GetBufferPointer(), program->GetBlob()->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&reflector);
		D3D12_SHADER_DESC desc;
		reflector->GetDesc(&desc);

		for (size_t i = 0; i < desc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflector->GetResourceBindingDesc(i, &bindDesc);

			switch (bindDesc.Type)
			{
			case D3D_SIT_CBUFFER:
			{
				m_rootParameterIndexMap[StringToWString(bindDesc.Name)] = static_cast<uint32_t>(m_rootParameters.size());

				ID3D12ShaderReflectionConstantBuffer* reflectedConstantBuffer = reflector->GetConstantBufferByName(bindDesc.Name);
				D3D12_SHADER_BUFFER_DESC bufferDesc;
				reflectedConstantBuffer->GetDesc(&bufferDesc);

				D3D12_ROOT_PARAMETER1 rootParameter;
				
				rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				rootParameter.Descriptor = {
					bindDesc.BindPoint,//ShaderRegister
					bindDesc.Space, //RegisterSpace
					D3D12_ROOT_DESCRIPTOR_FLAG_NONE, //Flags
				};
				rootParameter.ShaderVisibility = program->GetVisibility();

				m_rootParameters.push_back(rootParameter);

				break;
			}
			case D3D_SIT_TBUFFER:
			{
				//TODO
				break;
			}
			case D3D_SIT_TEXTURE:
			{
				m_rootParameterIndexMap[StringToWString(bindDesc.Name)] = static_cast<uint32_t>(m_rootParameters.size());

				ID3D12ShaderReflectionConstantBuffer* reflectedConstantBuffer = reflector->GetConstantBufferByName(bindDesc.Name);
				D3D12_SHADER_BUFFER_DESC bufferDesc;
				reflectedConstantBuffer->GetDesc(&bufferDesc);

				const CD3DX12_DESCRIPTOR_RANGE1 srvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
					1u,
					bindDesc.BindPoint,
					bindDesc.Space,
					D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

				m_descriptorRanges.push_back(srvRange);

				D3D12_ROOT_PARAMETER1 rootParameter;

				rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParameter.DescriptorTable = {
					1u,//NumDescriptorRanges
					&m_descriptorRanges.back() //pDescriptorRanges 
				};
				rootParameter.ShaderVisibility = program->GetVisibility();

				m_rootParameters.push_back(rootParameter);

				break;
			}
			case D3D_SIT_SAMPLER:
			{
				//TODO
				auto& sampler = m_samplers.emplace_back();
				sampler.Name = bindDesc.Name;
				sampler.BindPoint = bindDesc.BindPoint;
				sampler.Visibility = program->GetVisibility();
				break;
			}
			}
		}

		reflector->Release();
	}
}