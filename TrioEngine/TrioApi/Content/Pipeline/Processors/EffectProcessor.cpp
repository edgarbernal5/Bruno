#include "stdafx.h"
#include "EffectProcessor.h"

#include "Content/Pipeline/Graphics/EffectContent.h"
#include "Graphics/EffectCompiler.h"

namespace TrioEngine
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

		return input;
	}
}