#include "GameContentBuilder.h"

#include "ProcessorManager.h"
#include <Bruno/Core/FileStream.h>
#include "Serialization/ContentCompiler.h"

#include "TimestampCache.h"
#include <sstream>
#include <Bruno/Core/Log.h>
#include <Bruno/Core/StringHelpers.h>

namespace Bruno
{
	GameContentBuilder::GameContentBuilder()
	{
		PreparePaths();

		m_timestampCache = new TimestampCache();
	}

	GameContentBuilder::GameContentBuilder(const Settings& settings, TimestampCache* timestampCache) :
		m_settings(settings)
	{
		PreparePaths();

		if (!timestampCache)
		{
			timestampCache = new TimestampCache();
		}
		m_timestampCache = timestampCache;
	}

	std::wstring GameContentBuilder::GetAbsolutePath(const std::wstring& path)
	{
		std::filesystem::path fsPath(path);
		if (fsPath.has_root_path())
			return path;

		return std::filesystem::path(m_settings.RootDirectory) / path;
	}

	std::wstring GameContentBuilder::GetRelativePath(const std::wstring& path)
	{
		//BR_ASSERT path is not absolute
		if (!path._Starts_with(m_settings.RootDirectory))
			return path;

		return std::filesystem::relative(path, m_settings.RootDirectory).wstring();
	}

	BuildItem* GameContentBuilder::RequestBuild(const std::filesystem::path& sourceFilename, const std::wstring& assetName, const std::string& processorName)
	{
		if (!m_timestampCache->FileExists(sourceFilename))
		{
			std::ostringstream error;
			error << "Source asset not found. " << sourceFilename;
			throw std::exception(error.str().c_str());
		}

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
		newRequest->SourceFilename = GetRelativePath(sourceFilename.c_str());
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

			auto absolutePath = GetAbsolutePath(itemToBuild->OutputFilename);
			m_timestampCache->Remove(absolutePath);
			std::filesystem::remove(absolutePath);

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
			if (!sourceFilePath.has_root_path())
			{
				auto directoryName = std::filesystem::path(request.SourceFilename).parent_path().wstring();
				auto relativePath = GetRelativePath(m_settings.IntermediateDirectory);
				
				if (!directoryName.empty() && directoryName._Starts_with(relativePath))
				{
					directoryName = directoryName.substr(relativePath.size());
				}
				if (!directoryName.empty())
				{
					outputDirectory = std::filesystem::path(outputDirectory) / directoryName;
				}
			}

			std::wstring assetNameStub = sourceFilePath.stem();
			if (assetNameStub.size() > 32)
			{
				assetNameStub = assetNameStub.substr(0, 32);
			}

			uint32_t indexing = 0;
			do
			{
				std::wstringstream pathBuilder;
				pathBuilder << outputDirectory << L"\\" << assetNameStub << '_' << indexing << OutputExtention;

				outputFilename = GetRelativePath(pathBuilder.str());

				++indexing;
			} while (FindBuildItemByOutputFilename(outputFilename) != nullptr);
		}
		else
		{
			std::filesystem::path outputPath = outputDirectory;
			outputPath /= request.AssetName;
			outputPath += OutputExtention;

			outputFilename = GetRelativePath(outputPath);
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
		std::string buildReason;
		if (!NeedsIncrementalBuild(buildItem, buildReason))
		{
			std::ostringstream message;
			message << "Asset is already up-to-date. " << WStringToString(buildItem.OutputFilename);
			BR_CORE_TRACE << message.str() << std::endl;
			return;
		}

		auto absolutePath = GetAbsolutePath(buildItem.Request->SourceFilename);
		if (!m_timestampCache->FileExists(absolutePath))
		{
			std::ostringstream error;
			error << "Source asset not found. " << WStringToString(buildItem.Request->SourceFilename);
			throw std::exception(error.str().c_str());
		}

		BR_CORE_TRACE << "Building item... Source = " << WStringToString(buildItem.Request->SourceFilename)
			<< ". Output = " << WStringToString(buildItem.OutputFilename) << std::endl;

		if (!buildReason.empty())
			BR_CORE_TRACE << buildReason << std::endl;

		buildItem.SourceTimestamp = m_timestampCache->GetTimestamp(absolutePath);
		auto processor = ProcessorManager::GetProcessorByName(buildItem.Request->ProcessorName);

		ContentProcessorContext context(this, &buildItem);
		auto output = processor->Process(absolutePath, context);
		
		buildItem.IsBuilt = true;
		SerializeAsset(buildItem, *output.get());
	}

	bool GameContentBuilder::NeedsIncrementalBuild(BuildItem& item, std::string& buildReason)
	{
		if (m_settings.RebuildAllAssets)
		{
			buildReason = "Rebuild all assets.";
			return true;
		}

		auto absolutePath = GetAbsolutePath(item.Request->SourceFilename);
		auto timestamp = m_timestampCache->GetTimestamp(absolutePath);
		if (timestamp != item.SourceTimestamp)
		{
			std::ostringstream reasonBuilder;
			reasonBuilder << "Rebuild dirty dependency. " << WStringToString(item.Request->SourceFilename);
			buildReason = reasonBuilder.str();
			return true;
		}

		if (!m_timestampCache->FileExists(GetAbsolutePath(item.OutputFilename)))
		{
			std::ostringstream reasonBuilder;
			reasonBuilder << "Rebuild missing output. " << WStringToString(item.OutputFilename);
			buildReason = reasonBuilder.str();
			return true;
		}
		return false;
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
		auto absolutePath = GetAbsolutePath(buildItem.OutputFilename);
		m_timestampCache->Remove(absolutePath);

		std::ostringstream message;
		message << "Compiling... " << WStringToString(absolutePath);
		BR_CORE_TRACE << message.str() << std::endl;

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