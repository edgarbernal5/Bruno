#include "brpch.h"
#include "RootSignature.h"

#include "GraphicsDevice.h"
#include "Shader.h"
#include "ShaderProgram.h"

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
	}

	RootSignature::RootSignature(Shader* shader)
	{
		ID3DBlob* code = shader->GetShaderProgram(Shader::ShaderProgramType::Vertex)->GetBlob();
	}

    RootSignature::~RootSignature()
    {
    }
}