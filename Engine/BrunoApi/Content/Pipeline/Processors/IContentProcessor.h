#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace BrunoEngine
{
	class ContentItem;
	class ContentProcessorContext;

	class BRUNO_API_EXPORT IContentProcessor
	{
	public:
		virtual ContentItem* Process(ContentItem* input, ContentProcessorContext* context) = 0;

		friend class GameContentBuilder;

		virtual std::string GetProcessorName() const = 0;
		virtual std::string GetOutputWriterName() const = 0;
	};
}