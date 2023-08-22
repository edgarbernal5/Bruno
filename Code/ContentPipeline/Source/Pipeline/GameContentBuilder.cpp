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

	BuildItem* GameContentBuilder::RequestBuild(const std::filesystem::path& sourceFilename, const std::wstring& assetName, const std::string& processorName)
	{
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

		BuildRequest* newRequest = new BuildRequest();
		newRequest->SourceFilename = sourceFilename.c_str();
		newRequest->AssetName = assetName;
		newRequest->ProcessorName = newProcessorName;

		BuildItem* itemToBuild = nullptr;
		for (size_t i = 0; i < m_buildItems.size(); i++)
		{
			auto request = m_buildItems.at(i);
			if (request->Request->AssetName == newRequest->AssetName &&
				request->Request->SourceFilename == newRequest->SourceFilename &&
				request->Request->ProcessorName == newRequest->ProcessorName)
			{
				itemToBuild = request;
				break;
			}
		}

		if (itemToBuild != nullptr && itemToBuild->Request->AssetName.empty() && !newRequest->AssetName.empty()) {
			//m_buildItems.erase
			auto it = std::find(m_buildItems.begin(), m_buildItems.end(), itemToBuild);
			if (it != m_buildItems.end())
				m_buildItems.erase(it);

			delete itemToBuild;
			itemToBuild = nullptr;
		}

		if (itemToBuild == nullptr)
		{
			itemToBuild = new BuildItem();
			itemToBuild->Request = newRequest;
			itemToBuild->IsBuilt = false;
			itemToBuild->OutputFilename = ChooseOutputFilename(*newRequest);

			m_buildItems.push_back(itemToBuild);
			m_requests.push_back(newRequest);
		}
		else {
			delete newRequest; //TODO: make it shared ptr
		}

		return itemToBuild;
	}

	void GameContentBuilder::Run()
	{
		for (size_t i = 0; i < m_buildItems.size(); i++)
		{
			auto& item = m_buildItems[i];
			if (item->IsBuilt)
				continue;

			BuildAsset(*item);
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
			uint32_t indexing = 0;
			do {
				std::wstringstream ss;
				ss << assetNameStub << '_' << indexing << OutputExtention;

				outputFilename = ss.str();

				++indexing;
			} while (FindBuildItemByOutputFilename(outputFilename) != nullptr);
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

	BuildItem* GameContentBuilder::FindBuildItemByOutputFilename(const std::wstring& outputFilename)
	{
		for (size_t i = 0; i < m_buildItems.size(); i++)
		{
			if (m_buildItems[i]->OutputFilename == outputFilename)
				return m_buildItems[i];
		}
		return nullptr;
	}

	void GameContentBuilder::BuildAsset(BuildItem& buildItem)
	{
		auto processor = ProcessorManager::GetProcessorByName(buildItem.Request->ProcessorName);

		ContentProcessorContext context(this, &buildItem);
		auto output = processor->Process(buildItem.Request->SourceFilename, context);
		
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
		compiler.Compile(fileStream, contentItem, true, m_settings.OutputDirectory, absolutePath);
	}

	BuildItem::BuildItem() :
		IsBuilt(false),
		Request(nullptr),
		OutputFilename{}
	{
	}
}