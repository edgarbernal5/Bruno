#include "stdafx.h"
#include "EffectImporter.h"

#include "Content/Pipeline/Graphics/EffectContent.h"

namespace TrioEngine
{
	EffectImporter::EffectImporter()
	{
	}

	EffectImporter::~EffectImporter()
	{
	}

	ContentItem* EffectImporter::Import(std::string& filename)
	{
		EffectContent* content = new EffectContent(filename);

		return content;
	}
}