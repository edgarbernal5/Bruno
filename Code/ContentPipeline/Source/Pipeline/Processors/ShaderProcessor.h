#pragma once

#include "Pipeline/AbstractProcessor.h"

#include <wrl/client.h>
#include <d3dcompiler.h>
#include <vector>

namespace Bruno
{
	class ShaderProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(ShaderProcessor, AbstractProcessor);

	public:
		ShaderProcessor();

		virtual std::shared_ptr<ContentItem> Process(const std::wstring& assetFilename, ContentProcessorContext& context) override;

	private:

		enum class ShaderProgramType : uint32_t
		{
			Vertex = 0,
			Pixel
		};

		struct ShaderTypeDesc
		{
			ShaderProgramType Type;
			const char* EntryPoint;
			const char* Target;
		};

		std::vector<uint8_t> m_compiledCodePrograms[2]{};

		bool CompileShader(const std::wstring& assetFilename, const std::string& entryPoint, const std::string& target, Microsoft::WRL::ComPtr<ID3DBlob>& compiledBlob, Microsoft::WRL::ComPtr<ID3DBlob>& errorBlob);

		static const ShaderTypeDesc& VS, & PS;
		static const ShaderTypeDesc ShaderTypes[];
	};
}
