#include "brpch.h"
#include "Shader.h"

namespace Bruno
{
	Shader::Shader(const std::wstring& pathToSource, std::string entryPoint, std::string target)
	{
#if defined(BR_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		uint32_t compileFlags = 0;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3DCompileFromFile(pathToSource.c_str(), 
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
}