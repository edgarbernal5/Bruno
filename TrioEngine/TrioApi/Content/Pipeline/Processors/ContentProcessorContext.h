#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ExternalReference.h"

namespace TrioEngine
{
	class ContentItem;

	class TRIO_API_EXPORT ContentProcessorContext
	{
	public:
		virtual std::string GetIntermediateDirectory() = 0;
		virtual std::string GetOutputDirectory() = 0;

		virtual OpaqueData* GetParameters() = 0;

		virtual void AddDependency(std::string filename) = 0;
		virtual void AddOutputFile(std::string filename) = 0;

		virtual ExternalReference BuildAsset(ExternalReference sourceAsset, std::string processorName, OpaqueData* processorParameters, std::string importerName, std::string assetName) = 0;

		virtual ContentItem* Convert(ContentItem* input, std::string processorName, OpaqueData *processorParameters) = 0;
	};
}