#include "GameContentBuilder.h"

#include "ProcessorManager.h"

namespace Bruno
{
	GameContentBuilder::GameContentBuilder(const Settings& settings) :
		m_settings(settings)
	{

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
				//newRequest.ProcessorName = guessProcessor->
			}
		}

	}

	void GameContentBuilder::Run()
	{
	}
}