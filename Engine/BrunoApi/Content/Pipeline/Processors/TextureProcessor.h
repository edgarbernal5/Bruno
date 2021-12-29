#pragma once

#include "TrioApiRequisites.h"

#include "IContentProcessor.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT TextureProcessor : public IContentProcessor
	{
	public:
		TextureProcessor();
		~TextureProcessor();

		ContentItem* Process(ContentItem* input, ContentProcessorContext *context);

		friend class BuildCoordinator;

		std::string GetProcessorName() const { return "TextureProcessor"; }
		std::string GetOutputWriterName() const { return "Texture2DWriter"; }

	private:
		
	};
}