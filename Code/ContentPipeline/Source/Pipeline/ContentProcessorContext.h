#pragma once

namespace Bruno
{
	class GameContentBuilder;
	struct BuildItem;
	struct ContentItem;

	class ContentProcessorContext
	{
	public:
		ContentProcessorContext(GameContentBuilder* builder, BuildItem* buildItem);
		std::shared_ptr<ContentItem> Convert(ContentItem* item, const std::string& processorName);

	private:
		GameContentBuilder* m_gameContentBuilder;
		BuildItem* m_currentBuildItem;
	};

}
