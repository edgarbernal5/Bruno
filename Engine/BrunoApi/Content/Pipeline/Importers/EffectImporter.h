#pragma once

#include "TrioApiRequisites.h"

#include "IContentImporter.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT EffectImporter : public IContentImporter
	{
	public:
		EffectImporter();
		~EffectImporter();

		ContentItem* Import(const std::string& filename);

		friend class GameContentBuilder;

		std::string GetImporterName() const { return "EffectImporter"; }
		std::string GetFileExtensions() const { return "fx"; }
	};
}