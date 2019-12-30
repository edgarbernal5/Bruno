#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioEngine
{
	class ContentItem;

	class TRIO_API_EXPORT IContentImporter
	{
	public:
		virtual ContentItem* Import(std::string& filename) = 0;

		friend class BuildCoordinator;

		virtual std::string GetImporterName() const = 0;
		virtual std::string GetFileExtensions() const = 0;
	};
}