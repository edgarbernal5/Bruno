#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace Bruno
{

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

		inline void SetSettings(const Settings& settings)
		{
			m_settings = settings;
			PreparePaths();
		}
		void RequestBuild(const std::filesystem::path& filename, const std::string& processorName = "");
		void Run();

	private:
		struct BuildRequest
		{
			std::wstring	AssetName;
			std::string		ProcessorName;
			bool IsBuilt	{ false };

			bool operator==(const BuildRequest& other)
			{
				return AssetName == other.AssetName && ProcessorName == other.ProcessorName;
			}
		};
		//struct BuildItem
		//{
		//	BuildRequest Request;
		//	bool IsBuilt{ false };
		//};
		Settings m_settings;

		std::vector<BuildRequest> m_requests;

		void PreparePaths();
	};
}

