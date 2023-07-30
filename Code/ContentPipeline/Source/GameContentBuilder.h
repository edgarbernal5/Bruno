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
			std::string IntermediateDirectory;
			std::string OutputDirectory;
			std::string RootDirectory;
		};

		GameContentBuilder() = default;
		GameContentBuilder(const Settings& settings);

		inline void SetSettings(const Settings& settings)
		{
			m_settings = settings;
		}
		void RequestBuild(const std::filesystem::path& filename, const std::string& processorName = "");
		void Run();

	private:
		struct BuildRequest
		{
			std::wstring AssetName;
			std::string ProcessorName;

			bool operator==(const BuildRequest& other)
			{
				return AssetName == other.AssetName && ProcessorName == other.ProcessorName;
			}
		};
		Settings m_settings;

		std::vector<BuildRequest> m_requests;
	};
}

