#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "AbstractProcessor.h"

namespace Bruno
{
	struct BuildRequest
	{
		std::wstring	SourceFilename;
		std::wstring	AssetName;
		std::string		ProcessorName;

		bool operator==(const BuildRequest& other)
		{
			return AssetName == other.AssetName && ProcessorName == other.ProcessorName;
		}
	};
	struct BuildItem
	{
		BuildRequest	Request;
		bool IsBuilt{ false };
		std::wstring	OutputFilename;

		bool operator==(const BuildItem& other)
		{
			return Request == other.Request && IsBuilt == other.IsBuilt;
		}
	};

	class GameContentBuilder
	{
	public:
		struct Settings
		{
			std::wstring RootDirectory;
			std::wstring IntermediateDirectory;
			std::wstring OutputDirectory;
		};

		GameContentBuilder() = default;
		GameContentBuilder(const Settings& settings);

		std::wstring GetAbsolutePath(const std::wstring path);

		inline void SetSettings(const Settings& settings)
		{
			m_settings = settings;
			PreparePaths();
		}
		void RequestBuild(const std::filesystem::path& sourceFilename, const std::wstring& assetName, const std::string& processorName = "");
		void Run();

		const std::wstring OutputExtention = L".bruno";
	private:
		Settings m_settings;

		std::vector<BuildRequest> m_requests;
		std::vector<BuildItem> m_buildItems;

		void BuildAsset(BuildItem& item);
		void PreparePaths();
		void SerializeAsset(const BuildItem& buildItem, const ContentItem& contentItem);
		std::wstring ChooseOutputFilename(const BuildRequest& request);
		BuildItem* FindBuildItemByFilename(const std::wstring& outputFilename);

		friend class ContentProcessorContext;
	};
}

