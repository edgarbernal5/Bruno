#include "stdafx.h"
#include "TextureProcessor.h"

#include "Content/Pipeline/Graphics/PixelBitmapContent.h"
#include "Content/Pipeline/Graphics/Texture2DContent.h"

namespace BrunoEngine
{
	TextureProcessor::TextureProcessor()
	{
	}

	TextureProcessor::~TextureProcessor()
	{
	}

	ContentItem* TextureProcessor::Process(ContentItem* input, ContentProcessorContext *context)
	{
		TextureContent* inputContent = reinterpret_cast<TextureContent*>(input);

		inputContent->ConvertBitmapType();
		return input;
	}
}