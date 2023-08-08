#pragma once

#include <string>
#include <memory>

#include "ExternalReferenceContentItem.h"

namespace Bruno
{
	class GameContentBuilder;
	struct BuildItem;
	struct ContentItem;

	class ContentProcessorContext
	{
	public:
		ContentProcessorContext(GameContentBuilder* builder, BuildItem* buildItem);

		ExternalReferenceContentItem BuildAsset(ExternalReferenceContentItem sourceAsset, const std::string& processorName, const std::wstring& assetName);

	private:
		GameContentBuilder* m_gameContentBuilder;
		BuildItem* m_currentBuildItem;
	};

}
