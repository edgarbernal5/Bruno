#include "stdafx.h"
#include "EffectImporter.h"

#include "Content/Pipeline/Graphics/EffectContent.h"

namespace BrunoEngine
{
	EffectImporter::EffectImporter()
	{
	}

	EffectImporter::~EffectImporter()
	{
	}

	ContentItem* EffectImporter::Import(const std::string& filename)
	{
		EffectContent* content = new EffectContent(const_cast<std::string&>(filename));

		return content;
	}
}