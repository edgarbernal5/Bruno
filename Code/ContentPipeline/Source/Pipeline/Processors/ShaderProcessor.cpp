#include "ShaderProcessor.h"

#include "Pipeline/Graphics/ShaderContentItem.h"
#include <Bruno/Core/Log.h>

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderProcessor);

	Bruno::ShaderProcessor::ShaderProcessor()
	{
		m_supportedExtensions = { L".hlsl", L".fx" };
	}

	std::shared_ptr<ContentItem> ShaderProcessor::Process(const std::wstring& assetFilename, ContentProcessorContext& context)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> compiledBlob;
		
		if (!CompileShader(assetFilename, "", "", compiledBlob, errorBlob))
			return nullptr;

		auto output = std::make_shared<ShaderContentItem>();
		output->CompiledCodeData.resize(compiledBlob->GetBufferSize());
		memcpy(output->CompiledCodeData.data(), compiledBlob->GetBufferPointer(), compiledBlob->GetBufferSize());

		return output;
	}

	bool ShaderProcessor::CompileShader(const std::wstring& assetFilename, const std::string& entryPoint, const std::string& target, Microsoft::WRL::ComPtr<ID3DBlob>& compiledBlob, Microsoft::WRL::ComPtr<ID3DBlob>& errorBlob)
	{
#if defined(BR_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		uint32_t compileFlags = 0;
#endif

		HRESULT hr = D3DCompileFromFile(assetFilename.c_str(),
			nullptr,
			nullptr,
			entryPoint.c_str(),
			target.c_str(),
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
			return false;
		}

		return true;
	}
}