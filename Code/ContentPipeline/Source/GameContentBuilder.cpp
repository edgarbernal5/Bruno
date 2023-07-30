#include "GameContentBuilder.h"

#include "ProcessorManager.h"

namespace Bruno
{
	GameContentBuilder::GameContentBuilder(const Settings& settings) :
		m_settings(settings)
	{
		PreparePaths();
	}

	void GameContentBuilder::RequestBuild(const std::filesystem::path& filename, const std::string& processorName)
	{
		auto it = std::find(m_requests.begin(), m_requests.end(), BuildRequest{ filename , processorName });
		if (it != m_requests.end())
			return;

		auto& newRequest = m_requests.emplace_back();
		newRequest.AssetName = filename.c_str();
		newRequest.ProcessorName = processorName;
		if (processorName.empty())
		{
			auto guessProcessor = ProcessorManager::GetProcessorByExtension(filename.extension());
			if (guessProcessor)
			{
				newRequest.ProcessorName = guessProcessor->ToString();
			}
		}
		if (newRequest.ProcessorName.empty())
			throw std::exception("Invalid parameter of processor name.");

		newRequest.IsBuilt = false;
	}

	void GameContentBuilder::Run()
	{
	}

	void GameContentBuilder::PreparePaths()
	{
		if (m_settings.RootDirectory.empty())
		{
			m_settings.RootDirectory = std::filesystem::current_path();
		}

		if (m_settings.IntermediateDirectory.empty())
		{
			m_settings.IntermediateDirectory = std::filesystem::path(m_settings.RootDirectory) / "tempObjects";
			
			if (!std::filesystem::exists(m_settings.IntermediateDirectory))
				std::filesystem::create_directory(m_settings.IntermediateDirectory);
		}
		
		if (m_settings.OutputDirectory.empty())
		{
			m_settings.OutputDirectory = std::filesystem::path(m_settings.RootDirectory) / "outputObjects";
			if (!std::filesystem::exists(m_settings.OutputDirectory))
				std::filesystem::create_directory(m_settings.OutputDirectory);
		}
	}
}