#pragma once

#include "TrioApiRequisites.h"

#include "IContentProcessor.h"

#include "Content/Pipeline/ExternalReference.h"
#include <string>

namespace TrioEngine
{
	class MaterialContent;

	class BRUNO_API_EXPORT MaterialProcessor : public IContentProcessor
	{
	public:
		MaterialProcessor();
		~MaterialProcessor();

		ContentItem* Process(ContentItem* input, ContentProcessorContext *context);

		friend class BuildCoordinator;

		std::string GetProcessorName() const { return "MaterialProcessor"; }
		std::string GetOutputWriterName() const { return "MaterialWriter"; }

	private:
		void BuildAllTextures(MaterialContent* input, ContentProcessorContext* context);

	protected:
		virtual ExternalReference BuildTexture(std::string textureName, ExternalReference texture, ContentProcessorContext* context);

		virtual ExternalReference BuildEffect(ExternalReference effect, ContentProcessorContext* context);
	};
}