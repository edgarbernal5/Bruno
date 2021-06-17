#pragma once

#include "TrioApiRequisites.h"

#include "IContentProcessor.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT EffectProcessor : public IContentProcessor
	{
	public:
		EffectProcessor();
		~EffectProcessor();

		ContentItem* Process(ContentItem* input, ContentProcessorContext *context);

		friend class BuildCoordinator;

		std::string GetProcessorName() const { return "EffectProcessor"; }
		std::string GetOutputWriterName() const { return "Texture2DWriter"; }

	private:
		
	};
}