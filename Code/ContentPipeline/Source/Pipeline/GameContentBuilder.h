#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "AbstractProcessor.h"

namespace Bruno
{
	struct BuildRequest
	{
		BuildRequest() = default;

		std::wstring	SourceFilename;
		std::wstring	AssetName;
		std::string		ProcessorName;

		bool operator==(const BuildRequest* other)
		{
			return AssetName == other->AssetName && ProcessorName == other->ProcessorName;
		}
	};

	struct BuildItem
	{
		BuildItem();

		BuildRequest*	Request;
		bool IsBuilt{ false };
		std::wstring	OutputFilename;
		std::filesystem::file_time_type SourceTimestamp;

		bool operator==(const BuildItem* other)
		{
			return Request == other->Request && IsBuilt == other->IsBuilt;
		}
	};

	class TimestampCache;

	class GameContentBuilder
	{
	public:
		struct Settings
		{
			std::wstring RootDirectory;
			std::wstring IntermediateDirectory;
			std::wstring OutputDirectory;

			std::wstring CacheFilename;
			bool RebuildAllAssets{ false };
		};

		GameContentBuilder();
		GameContentBuilder(const Settings& settings, TimestampCache* timestampCache = nullptr);

		std::wstring GetAbsolutePath(const std::wstring& path);
		std::wstring GetRelativePath(const std::wstring& path);

		inline void SetSettings(const Settings& settings)
		{
			m_settings = settings;
			PreparePaths();
		}
		BuildItem* RequestBuild(const std::filesystem::path& sourceFilename, const std::wstring& assetName, const std::string& processorName = "");
		void Run();

		const std::wstring OutputExtention = L".bruno";

		friend class ContentProcessorContext;
	private:
		void BuildAsset(BuildItem& item);
		bool NeedsIncrementalBuild(BuildItem& item, std::string& buildReason);
		void PreparePaths();
		void SerializeAsset(const BuildItem& buildItem, const ContentItem& contentItem);
		std::wstring ChooseOutputFilename(const BuildRequest& request);
		BuildItem* FindBuildItemByOutputFilename(const std::wstring& outputFilename);

		Settings m_settings;
		TimestampCache* m_timestampCache;

		std::vector<BuildRequest*> m_requests;
		std::vector<BuildItem*> m_buildItems;
	};
}

