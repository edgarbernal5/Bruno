#include "brpch.h"
#include "RootSignature.h"

#include "D3DHelpers.h"
#include "GraphicsDevice.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "PipelineStateObject.h"

namespace Bruno
{
    RootSignature::RootSignature(const CD3DX12_ROOT_SIGNATURE_DESC& rootSignatureDesc)
        : m_rootSignatureDesc(rootSignatureDesc)
    {
		auto device = Graphics::GetDevice();

		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			BR_CORE_ERROR << "Error during root signature creation. " << (char*)errorBlob->GetBufferPointer() << std::endl;
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(device->GetD3DDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
    }

	RootSignature::RootSignature(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& versionedRootSignatureDesc)
	{
		auto device = Graphics::GetDevice();

		// Serialize the root signature.
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&versionedRootSignatureDesc,
			device->GetHighestRootSignatureVersion(), &rootSignatureBlob, &errorBlob));

		// Create the root signature.
		ThrowIfFailed(device->GetD3DDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
	}

	RootSignature::RootSignature(Shader* shader)
	{
		ID3DBlob* code = shader->GetShaderProgram(Shader::ShaderProgramType::Vertex)->GetBlob();


	}

	RootSignature::RootSignature(const PipelineResourceLayout& layout, PipelineResourceMapping& resourceMapping)
	{
        auto device = Graphics::GetDevice();

		std::vector<D3D12_ROOT_PARAMETER1> rootParameters;
		std::array<std::vector<D3D12_DESCRIPTOR_RANGE1>, Graphics::Core::RESOURCE_SPACES_COUNT> desciptorRanges;

        for (uint32_t spaceId = 0; spaceId < Graphics::Core::RESOURCE_SPACES_COUNT; spaceId++)
        {
            PipelineResourceSpace* currentSpace = layout.Spaces[spaceId];
            std::vector<D3D12_DESCRIPTOR_RANGE1>& currentDescriptorRange = desciptorRanges[spaceId];

            if (currentSpace)
            {
                const GpuBuffer* cbv = currentSpace->GetCBV();
                auto& uavs = currentSpace->GetUAVs();
                auto& srvs = currentSpace->GetSRVs();

                if (cbv)
                {
                    D3D12_ROOT_PARAMETER1 rootParameter{};
                    rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
                    rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                    rootParameter.Descriptor.RegisterSpace = spaceId;
                    rootParameter.Descriptor.ShaderRegister = 0;

                    resourceMapping.CbvMapping[spaceId] = static_cast<uint32_t>(rootParameters.size());
                    rootParameters.push_back(rootParameter);
                }

                if (uavs.empty() && srvs.empty())
                {
                    continue;
                }

                for (auto& uav : uavs)
                {
                    D3D12_DESCRIPTOR_RANGE1 range{};
                    range.BaseShaderRegister = uav.BindingIndex;
                    range.NumDescriptors = 1;
                    range.OffsetInDescriptorsFromTableStart = static_cast<uint32_t>(currentDescriptorRange.size());
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                    range.RegisterSpace = spaceId;
                    range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

                    currentDescriptorRange.push_back(range);
                }

                for (auto& srv : srvs)
                {
                    D3D12_DESCRIPTOR_RANGE1 range{};
                    range.BaseShaderRegister = srv.BindingIndex;
                    range.NumDescriptors = 1;
                    range.OffsetInDescriptorsFromTableStart = static_cast<uint32_t>(currentDescriptorRange.size());
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                    range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
                    range.RegisterSpace = spaceId;

                    currentDescriptorRange.push_back(range);
                }

                D3D12_ROOT_PARAMETER1 desciptorTableForSpace{};
                desciptorTableForSpace.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                desciptorTableForSpace.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                desciptorTableForSpace.DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(currentDescriptorRange.size());
                desciptorTableForSpace.DescriptorTable.pDescriptorRanges = currentDescriptorRange.data();

                resourceMapping.TableMapping[spaceId] = static_cast<uint32_t>(rootParameters.size());
                rootParameters.push_back(desciptorTableForSpace);
            }
        }
        CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Desc_1_1.NumParameters = static_cast<uint32_t>(rootParameters.size());
        rootSignatureDesc.Desc_1_1.pParameters = rootParameters.data();
        rootSignatureDesc.Desc_1_1.NumStaticSamplers = 1;
        rootSignatureDesc.Desc_1_1.pStaticSamplers = &linearRepeatSampler;
        //rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
        rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        //CD3DX12_ROOT_SIGNATURE_DESC

        ID3DBlob* rootSignatureBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;
        ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &rootSignatureBlob, &errorBlob));

        ThrowIfFailed(device->GetD3DDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
	}

    RootSignature::~RootSignature()
    {
    }
}