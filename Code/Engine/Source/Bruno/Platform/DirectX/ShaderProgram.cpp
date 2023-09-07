#include "brpch.h"
#include "ShaderProgram.h"

#include "VertexTypes.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderProgram);

	ShaderProgram::ShaderProgram(const std::wstring& sourceFilename, const std::string& entryPoint, const  std::string& target, D3D12_SHADER_VISIBILITY visibility) :
		m_visibility(visibility)
	{
#if defined(BR_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		uint32_t compileFlags = 0;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3DCompileFromFile(sourceFilename.c_str(),
			nullptr, 
			nullptr, 
			entryPoint.c_str(), //"main", 
			target.c_str(), //"vs_5_0", 
			compileFlags, 
			0, 
			m_blob.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				BR_CORE_ERROR << "Compilation error. " << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
		}
	}

	ShaderProgram::ShaderProgram(const std::vector<uint8_t>& compiledCode, D3D12_SHADER_VISIBILITY visibility) :
		m_visibility(visibility)
	{
		D3DCreateBlob(compiledCode.size(), &m_blob);
		CopyMemory(m_blob->GetBufferPointer(), compiledCode.data(), compiledCode.size());
	}

	D3D12_INPUT_LAYOUT_DESC ShaderProgram::GetInputLayout()
	{
		ID3D12ShaderReflection* reflector = nullptr;
		D3DReflect(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&reflector);

		D3D12_SHADER_DESC desc;
		reflector->GetDesc(&desc);

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
		if (GetBuiltInInputLayout(desc, reflector, inputLayoutDesc))
		{
			reflector->Release();
			return inputLayoutDesc;
		}

		inputLayoutDesc.NumElements = desc.InputParameters;
		for (uint32_t i = 0; i < desc.InputParameters; i++)
		{
			D3D12_SIGNATURE_PARAMETER_DESC parameterDesc;
			reflector->GetInputParameterDesc(i, &parameterDesc);

			auto aa = parameterDesc.SemanticName;
		}
		reflector->Release();

		return inputLayoutDesc;
	}

	bool ShaderProgram::GetBuiltInInputLayout(const D3D12_SHADER_DESC& desc, ID3D12ShaderReflection* reflector, D3D12_INPUT_LAYOUT_DESC& inputLayout)
	{
		std::string codedVertexType;
		for (uint32_t i = 0; i < desc.InputParameters; i++)
		{
			D3D12_SIGNATURE_PARAMETER_DESC parameterDesc;
			reflector->GetInputParameterDesc(i, &parameterDesc);

			codedVertexType += parameterDesc.SemanticName[0];
		}

		if (VertexTypesGetter::Get(std::hash<std::string>{}(codedVertexType), inputLayout))
		{
			return true;
		}
		
		return false;
	}
	
	DXGI_FORMAT ShaderProgram::GetFormat(BYTE mask, D3D_REGISTER_COMPONENT_TYPE componentType)
	{
		if (mask == 1) {
			if (componentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32_UINT;
			else if (componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32_SINT;
			else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32_FLOAT;
		}
		else if (mask <= 3) {
			if (componentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32_UINT;
			else if (componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32_SINT;
			else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (mask <= 7) {
			if (componentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32_UINT;
			else if (componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32_SINT;
			else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (mask <= 15) {
			if (componentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32A32_UINT;
			else if (componentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32A32_SINT;
			else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
}