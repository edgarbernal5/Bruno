#include "GameContentBuilder.h"

#include "ProcessorManager.h"
#include <Bruno/Core/FileStream.h>
#include "Serialization/ContentCompiler.h"

namespace Bruno
{
	GameContentBuilder::GameContentBuilder(const Settings& settings) :
		m_settings(settings)
	{
		PreparePaths();
	}

	void GameContentBuilder::RequestBuild(const std::filesystem::path& sourceFilename, const std::wstring& assetName, const std::string& processorName)
	{
		auto it = std::find(m_requests.begin(), m_requests.end(), BuildRequest{ sourceFilename, assetName , processorName });
		if (it != m_requests.end())
			return;

		std::string newProcessorName = processorName;
		if (processorName.empty())
		{
			auto guessProcessor = ProcessorManager::GetProcessorByExtension(sourceFilename.extension());
			if (guessProcessor)
			{
				newProcessorName = guessProcessor->ToString();
			}
		}
		if (newProcessorName.empty())
			throw std::exception("Invalid parameter, processor name.");

		auto& newRequest = m_requests.emplace_back();
		newRequest.SourceFilename = sourceFilename.c_str();
		newRequest.AssetName = assetName;
		newRequest.ProcessorName = newProcessorName;

		auto& buildItem = m_buildItems.emplace_back();
		buildItem.Request = newRequest;
		buildItem.IsBuilt = false;

		buildItem.OutputFilename = ChooseOutputFilename(newRequest);
	}

	void GameContentBuilder::Run()
	{
		for (size_t i = 0; i < m_buildItems.size(); i++)
		{
			auto& item = m_buildItems[i];
			if (item.IsBuilt)
				continue;

			BuildAsset(item);
		}
	}

	std::wstring GameContentBuilder::ChooseOutputFilename(const BuildRequest& request)
	{
		std::wstring outputFilename;
		std::wstring outputDirectory = m_settings.OutputDirectory;

		if (request.AssetName.empty())
		{

		}
		else
		{
			std::filesystem::path outputPath = outputDirectory;
			outputPath /= request.AssetName;
			outputPath += OutputExtention;

			outputFilename = std::filesystem::relative(outputPath, m_settings.RootDirectory);
		}
		return outputFilename;
	}

	void GameContentBuilder::BuildAsset(BuildItem& buildItem)
	{
		auto processor = ProcessorManager::GetProcessorByName(buildItem.Request.ProcessorName);
		auto output = processor->Process(buildItem.Request.SourceFilename);
		
		buildItem.IsBuilt = true;
		SerializeAsset(buildItem, *output.get());
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
		}
		if (!std::filesystem::exists(m_settings.IntermediateDirectory))
		{
			std::filesystem::create_directory(m_settings.IntermediateDirectory);
		}			
		
		if (m_settings.OutputDirectory.empty())
		{
			m_settings.OutputDirectory = std::filesystem::path(m_settings.RootDirectory) / "outputObjects";
		}
		if (!std::filesystem::exists(m_settings.OutputDirectory))
		{
			std::filesystem::create_directory(m_settings.OutputDirectory);
		}
	}

	void GameContentBuilder::SerializeAsset(const BuildItem& buildItem, const ContentItem& contentItem)
	{
		auto absolutePath = std::filesystem::path(m_settings.RootDirectory) / std::filesystem::path(buildItem.OutputFilename);
		FileStream fileStream(absolutePath, FileAccess::Write);
		
		ContentCompiler compiler;
		compiler.Compile(fileStream, contentItem, false, m_settings.OutputDirectory, absolutePath);
	}
}