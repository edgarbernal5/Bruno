#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace BrunoEngine
{
	class ContentItem;

	class BRUNO_API_EXPORT IContentImporter
	{
	public:
		virtual ContentItem* Import(const std::string& filename) = 0;

		friend class GameContentBuilder;

		virtual std::string GetImporterName() const = 0;
		virtual std::string GetFileExtensions() const = 0;
	};
}