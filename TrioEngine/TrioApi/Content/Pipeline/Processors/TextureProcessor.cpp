#include "stdafx.h"
#include "TextureProcessor.h"

#include "Content/Pipeline/Graphics/PixelBitmapContent.h"
#include "Content/Pipeline/Graphics/Texture2DContent.h"

namespace TrioEngine
{
	TextureProcessor::TextureProcessor()
	{
	}

	TextureProcessor::~TextureProcessor()
	{
	}

	ContentItem* TextureProcessor::Process(ContentItem* input, ContentProcessorContext *context)
	{
		//TextureContent* inputContent = static_cast<TextureContent*>(input);


		return input;
	}
}