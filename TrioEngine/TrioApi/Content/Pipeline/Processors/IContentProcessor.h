#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioEngine
{
	class ContentItem;
	class ContentProcessorContext;

	class TRIO_API_EXPORT IContentProcessor
	{
	public:
		virtual ContentItem* Process(ContentItem* input, ContentProcessorContext* context) = 0;

		friend class BuildCoordinator;

		virtual std::string GetProcessorName() const = 0;
		virtual std::string GetOutputWriterName() const = 0;
	};
}