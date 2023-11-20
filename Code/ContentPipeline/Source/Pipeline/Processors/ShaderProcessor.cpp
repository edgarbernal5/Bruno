#include "ShaderProcessor.h"

#include "Pipeline/Graphics/ShaderContentItem.h"
#include "Bruno/Core/Log.h"
#include "Bruno/Core/FileStream.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderProcessor);

	const ShaderProcessor::ShaderTypeDesc& ShaderProcessor::VS = { ShaderProgramType::Vertex, "main_VS", "vs_5_1" };
	const ShaderProcessor::ShaderTypeDesc& ShaderProcessor::PS = { ShaderProgramType::Pixel, "main_PS", "ps_5_1" };

	const ShaderProcessor::ShaderTypeDesc ShaderProcessor::ShaderTypes[] = { ShaderProcessor::VS, ShaderProcessor::PS };

	Bruno::ShaderProcessor::ShaderProcessor()
	{
		m_supportedExtensions = { L".hlsl", L".fx" };
	}

	std::shared_ptr<ContentItem> ShaderProcessor::Process(const std::wstring& assetFilename, ContentProcessorContext& context)
	{
		FileStream fileStream(assetFilename, FileAccess::Read);

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> compiledBlob;
		
		std::string content;
		std::vector<uint8_t> rawBytes;
		fileStream.ReadBytes(rawBytes, fileStream.GetLength());
		content = std::string(rawBytes.data(), rawBytes.data() + rawBytes.size());

		for (size_t i = 0; i < Graphics::Core::SHADER_PROGRAMS_COUNT; i++)
		{
			auto entryPointIndex = content.find(ShaderTypes[i].EntryPoint);
			if (entryPointIndex != std::string::npos)
			{
				if (!CompileShader(assetFilename, ShaderTypes[i].EntryPoint, ShaderTypes[i].Target, compiledBlob, errorBlob))
					return nullptr;

				m_compiledCodePrograms[i].resize(compiledBlob->GetBufferSize());
				memcpy(m_compiledCodePrograms[i].data(), compiledBlob->GetBufferPointer(), compiledBlob->GetBufferSize());
			}
		}

		auto output = std::make_shared<ShaderContentItem>();
		for (size_t i = 0; i < Graphics::Core::SHADER_PROGRAMS_COUNT; i++)
		{
			if (m_compiledCodePrograms[i].size() > 0)
			{
				output->CompiledCodeProgramsData[i] = std::move(m_compiledCodePrograms[i]);
			}
		}

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