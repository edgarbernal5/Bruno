#include "stdafx.h"
#include "EffectProcessor.h"

#include "Content/Pipeline/Graphics/CompiledEffectContent.h"
#include "Content/Pipeline/Graphics/EffectContent.h"
#include "Graphics/EffectCompiler.h"

namespace BrunoEngine
{
	EffectProcessor::EffectProcessor()
	{
	}

	EffectProcessor::~EffectProcessor()
	{
	}

	ContentItem* EffectProcessor::Process(ContentItem* input, ContentProcessorContext *context)
	{
		EffectContent* content = (EffectContent*)input;

#ifdef BRUNO_DIRECTX
#if defined(DEBUG) || defined(_DEBUG)
		bool forceOptimization = false;
#else
		bool forceOptimization = true;
#endif
		std::string filename(content->GetEffectCode().begin(), content->GetEffectCode().end());
		LPCSTR profile = "fx_5_0";

		ID3DBlob* compiledEffect = EffectCompiler::CompileShader(filename, nullptr, profile, nullptr, forceOptimization);
		
		if (compiledEffect == nullptr) {
			throw std::exception("effect error");
		}
		
		std::vector<uint8_t> bytes(compiledEffect->GetBufferSize());
		memcpy(bytes.data(), compiledEffect->GetBufferPointer(), compiledEffect->GetBufferSize());

#else
		std::vector<uint8_t> bytes;
#endif
		CompiledEffectContent* effectCompiled = new CompiledEffectContent(bytes);
		return effectCompiled;
	}
}