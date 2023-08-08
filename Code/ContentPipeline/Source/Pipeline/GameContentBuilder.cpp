#include "GameContentBuilder.h"

#include "ProcessorManager.h"
#include <Bruno/Core/FileStream.h>
#include "Serialization/ContentCompiler.h"

#include <sstream>

namespace Bruno
{
	GameContentBuilder::GameContentBuilder(const Settings& settings) :
		m_settings(settings)
	{
		PreparePaths();
	}

	std::wstring GameContentBuilder::GetAbsolutePath(const std::wstring path)
	{
		std::filesystem::path fsPath(path);
		if (fsPath.has_root_path())
			return path;

		return std::filesystem::path(m_settings.RootDirectory) / path;
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
			std::filesystem::path sourceFilePath(request.SourceFilename);
			if (!sourceFilePath.has_root_path()) {

			}

			std::wstring assetNameStub = sourceFilePath.stem();
			if (assetNameStub.size() > 32) {
				assetNameStub = assetNameStub.substr(0, 32);
			}
			int num = 0;
			do {
				std::wstringstream ss;
				ss << assetNameStub << '_' << num << OutputExtention;

				outputFilename = ss.str();

				++num;
			} while (FindBuildItemByFilename(outputFilename) != nullptr);

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

	BuildItem* GameContentBuilder::FindBuildItemByFilename(const std::wstring& outputFilename)
	{
		for (size_t i = 0; i < m_buildItems.size(); i++)
		{
			if (m_buildItems[i].OutputFilename == outputFilename)
				return &m_buildItems[i];
		}
		return nullptr;
	}

	void GameContentBuilder::BuildAsset(BuildItem& buildItem)
	{
		auto processor = ProcessorManager::GetProcessorByName(buildItem.Request.ProcessorName);

		ContentProcessorContext context(this, &buildItem);
		auto output = processor->Process(buildItem.Request.SourceFilename, context);
		
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
		std::filesystem::path fspath(buildItem.OutputFilename);

		auto absolutePath = std::filesystem::path(m_settings.RootDirectory) / std::filesystem::path(buildItem.OutputFilename);
		FileStream fileStream(absolutePath, FileAccess::Write);
		
		ContentCompiler compiler;
		compiler.Compile(fileStream, contentItem, false, m_settings.OutputDirectory, absolutePath);
	}
}