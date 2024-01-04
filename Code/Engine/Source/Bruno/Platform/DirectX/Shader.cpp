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

	Shader::Shader(std::array<std::vector<uint8_t>, Graphics::Core::SHADER_PROGRAMS_COUNT>&& programsData)
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

	std::shared_ptr<RootSignature> Shader::CreateRootSignature(PipelineResourceMapping& resourceMapping)
	{
		//CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc;
		//versionRootSignatureDesc.Init_1_1(static_cast<uint32_t>(m_rootParameters.size()),
		//	m_rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
				
		for (size_t i = 0; i < m_rootParameters.size(); i++)
		{
			auto& parameter = m_rootParameters[i];
			uint32_t spaceId = parameter.Descriptor.RegisterSpace;
			resourceMapping.CbvMapping[spaceId] = static_cast<uint32_t>(i);

			resourceMapping.TableMapping[spaceId] = static_cast<uint32_t>(i);
		}

		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
		samplers.resize(m_samplers.size());
		
		for (size_t i = 0; i < m_samplers.size(); i++)
		{
			samplers[i] = m_samplers[i].Desc;
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignature;
		versionedRootSignature.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		versionedRootSignature.Desc_1_1 =
		{
			static_cast<uint32_t>(m_rootParameters.size()), //NumParameters 
			m_rootParameters.data(), //pParameters
			static_cast<uint32_t>(samplers.size()), //NumStaticSamplers 
			samplers.data(), //pStaticSamplers 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,//Flags
		};

		return std::make_shared<RootSignature>(versionedRootSignature);
	}

	ShaderProgram* Shader::GetShaderProgram(ShaderProgramType type)
	{
		if (m_programs[(int)type])
			return m_programs[(int)type].get();

		return nullptr;
	}

	uint32_t Shader::GetIndexMap(const std::wstring& name)
	{
		return m_rootParameterIndexMap[name];
	}

	void Shader::InitializeParameters()
	{
		m_rootParameters.clear();
		m_descriptorRanges.clear();
		m_samplers.clear();

		for (size_t i = 0; i < Graphics::Core::SHADER_PROGRAMS_COUNT; i++)
		{
			if (m_programs[i])
			{
				InitializeParameters(m_programs[i].get());
			}
		}
	}

	void Shader::InitializeParameters(ShaderProgram* program)
	{
		ID3D12ShaderReflection* reflector = nullptr;
		D3DReflect(program->GetBlob()->GetBufferPointer(), program->GetBlob()->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&reflector);
		D3D12_SHADER_DESC desc;
		reflector->GetDesc(&desc);

		//TODO: sort root parameters by priority (most used comes first, example textures).
		for (uint32_t i = 0; i < desc.BoundResources; i++)
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

				CD3DX12_ROOT_PARAMETER1 rootParameter;
				
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

				CD3DX12_ROOT_PARAMETER1 rootParameter;

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
				auto& sampler = m_samplers.emplace_back();
				sampler.Name = bindDesc.Name;
				sampler.BindPoint = bindDesc.BindPoint;
				sampler.Visibility = program->GetVisibility();
				sampler.Setup();
				break;
			}
			}
		}

		reflector->Release();
	}

	void Shader::Sampler::Setup()
	{
		Desc.Filter = D3D12_FILTER_ANISOTROPIC;
		Desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		Desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		Desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		Desc.MipLODBias = 0;
		Desc.MaxAnisotropy = 16;
		Desc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		Desc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		Desc.MinLOD = 0.0f;
		Desc.MaxLOD = D3D12_FLOAT32_MAX;
		Desc.ShaderRegister = BindPoint;
		Desc.RegisterSpace = 0;
		Desc.ShaderVisibility = Visibility;
	}
}