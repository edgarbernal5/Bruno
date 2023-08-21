#pragma once

#include "Pipeline/AbstractProcessor.h"

#include <wrl/client.h>
#include <d3dcompiler.h>

namespace Bruno
{
	class ShaderProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(ShaderProcessor, AbstractProcessor);

	public:
		ShaderProcessor();

		virtual std::shared_ptr<ContentItem> Process(const std::wstring& assetFilename, ContentProcessorContext& context) override;

	private:
		struct ShaderEntry
		{
			std::string EntryPoint;
			std::string TargetPoint;
		};

		//ShaderEntry Entries[2]{ ShaderEntry{"main_vs", "vs_5_1"}, ShaderEntry{"main_ps", "ps_5_1"} };

		bool CompileShader(const std::wstring& assetFilename, const std::string& entryPoint, const std::string& target, Microsoft::WRL::ComPtr<ID3DBlob>& compiledBlob, Microsoft::WRL::ComPtr<ID3DBlob>& errorBlob);
	};
}
