#include "brpch.h"
#include "ShaderProgram.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderProgram);

	ShaderProgram::ShaderProgram(const std::wstring& sourceFilename, std::string entryPoint, std::string target)
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

	ShaderProgram::ShaderProgram(const std::vector<uint8_t>& compiledCode)
	{
		D3DCreateBlob(compiledCode.size(), &m_blob);
		CopyMemory(m_blob->GetBufferPointer(), compiledCode.data(), compiledCode.size());
	}
	
}