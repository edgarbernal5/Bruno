#include "ShaderProcessor.h"

#include "Pipeline/Graphics/ShaderContentItem.h"
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <Bruno/Core/Log.h>

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderProcessor);

	Bruno::ShaderProcessor::ShaderProcessor()
	{
		m_supportedExtensions = { L".hlsl", L".fx" };
	}

	std::shared_ptr<ContentItem> ShaderProcessor::Process(const std::wstring& assetFilename)
	{
#if defined(BR_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		uint32_t compileFlags = 0;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> compiledBlob;

		HRESULT hr = D3DCompileFromFile(assetFilename.c_str(),
			nullptr,
			nullptr,
			"main",		//entryPoint.c_str(), //"main", 
			"vs_5_0",		//target.c_str(), //"vs_5_0", 
			compileFlags,
			0,
			compiledBlob.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				BR_CORE_ERROR << "Compilation error. " << (char*)errorBlob->GetBufferPointer() << std::endl;

				std::ostringstream outputError;
				outputError << "Compilation error. " << (char*)errorBlob->GetBufferPointer() << std::endl;
				throw std::exception(outputError.str().c_str());
			}
		}

		auto output = std::make_shared<ShaderContentItem>();
		output->CompiledCodeData.resize(compiledBlob->GetBufferSize());
		memcpy(output->CompiledCodeData.data(), compiledBlob->GetBufferPointer(), compiledBlob->GetBufferSize());

		return output;
	}
}