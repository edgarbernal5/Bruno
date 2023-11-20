#pragma once

#include <d3d12.h>
#pragma comment(lib, "dxcompiler.lib")
#include <d3dcompiler.h>
#include <wrl.h>
#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	class ShaderProgram : public RTTI
	{
		BR_RTTI_DECLARATION(ShaderProgram, RTTI);

	public:
		ShaderProgram(const std::wstring& sourceFilename, const std::string& entryPoint, const  std::string& target, D3D12_SHADER_VISIBILITY visibility);
		ShaderProgram(const std::vector<uint8_t>& compiledCode, D3D12_SHADER_VISIBILITY visibility);

		ID3DBlob* GetBlob() { return m_blob.Get(); }

		D3D12_INPUT_LAYOUT_DESC GetInputLayout();
		inline D3D12_SHADER_VISIBILITY GetVisibility() { return m_visibility; }

	private:
		bool GetBuiltInInputLayout(const D3D12_SHADER_DESC& desc, ID3D12ShaderReflection* reflector, D3D12_INPUT_LAYOUT_DESC& inputLayout);
		DXGI_FORMAT GetFormat(BYTE mask, D3D_REGISTER_COMPONENT_TYPE componentType);

		Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
		D3D12_SHADER_VISIBILITY m_visibility;
	};
}

