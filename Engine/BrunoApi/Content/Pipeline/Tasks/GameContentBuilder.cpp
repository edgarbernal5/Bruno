#include "stdafx.h"
#include "GameContentBuilder.h"

#include "IO/FileStream.h"
#include "IO/File.h"
#include "Vfs/IStorage.h"

#include "Content/Pipeline/Serialization/ContentCompiler.h"
#include "Content/Pipeline/Importers/ImporterManager.h"
#include "Content/Pipeline/Importers/IContentImporter.h"
#include "Content/Pipeline/Processors/BasicContentProcessorContext.h"
#include "Content/Pipeline/Processors/ProcessorManager.h"
#include "Content/Pipeline/Processors/IContentProcessor.h"
#include "Content/Pipeline/Tasks/BuildItem.h"
#include "Content/Pipeline/Tasks/BuildItemCollection.h"
#include "Content/Pipeline/Tasks/BuildRequest.h"

#include "Errors/ContentPipelineException.h"

namespace BrunoEngine
{
	GameContentBuilder::GameContentBuilder(GameContentBuilderSettings* settings, TimestampCache* timestampCache) :
		m_importerManager(nullptr), m_processorManager(nullptr), m_buildItems(nullptr), m_buildItemsChanged(false),
		m_settings(settings)
	{
		m_settings->InitializePaths();

		m_importerManager = new ImporterManager();
		m_processorManager = new ProcessorManager();

		m_contentCompiler = new ContentCompiler();

		m_buildItems = new BuildItemCollection();

		if (timestampCache)
		{
			m_timestampCache = *timestampCache;
		}
	}

	GameContentBuilder::~GameContentBuilder()
	{
		if (m_importerManager != nullptr)
		{
			delete m_importerManager;
			m_importerManager = nullptr;
		}
		if (m_processorManager != nullptr)
		{
			delete m_processorManager;
			m_processorManager = nullptr;
		}
		if (m_contentCompiler != nullptr)
		{
			delete m_contentCompiler;
			m_contentCompiler = nullptr;
		}
		if (m_buildItems != nullptr)
		{
			delete m_buildItems;
			m_buildItems = nullptr;
		}
	}

	void GameContentBuilder::AddDependency(BuildItem* buildItem, std::string filename)
	{
		std::string relativePath = GetRelativePath(filename);
		auto timestamp = m_timestampCache.GetTimestamp(filename);

		buildItem->m_dependencies[relativePath] = timestamp;
	}

	void GameContentBuilder::RunTheBuild()
	{
		BuildItem* item = nullptr;
		m_buildItems->ReverseWantedItems(0);

		while ((item = m_buildItems->PopWantedItem()) != nullptr)
		{
			if (!item->m_isBuilt)
			{
				int wantedItemsCount = m_buildItems->WantedItemsCount();

				BuildAsset(item);
				//for each dependency in item
				for (auto& pair : item->m_dependencies)
				{
					BuildItem* item2 = m_buildItems->FindItem(pair.first);
					if (item2)
					{
						m_buildItems->WantItem(item2);
					}
				}

				//for item.Requests
				//wantitem
				for (auto& current : item->m_requests)
				{
					m_buildItems->WantItem(m_buildItems->FindItem(current));
				}

				m_buildItems->ReverseWantedItems(wantedItemsCount);
			}
		}

		m_buildItemsChanged |= m_buildItems->RemoveUnwantedItems();
	}

	ContentItem* GameContentBuilder::BuildAsset(BuildItem* item)
	{
		if (item->m_isBuilt)
		{
			return nullptr;
		}
		//log ?
		m_buildItemsChanged = true;
		item->m_dependencies.clear();
		item->m_requests.clear();
		item->m_extraOutputFiles.clear();

		ContentItem* input = ImportAsset(item);

		ContentItem* output = input;
		if (input != nullptr)
		{
			std::string& processorName = item->m_buildRequest->m_processorName;

			if (processorName.size() > 0)
			{
				ContentProcessorContext* context = new BasicContentProcessorContext(this, item, "");

				IContentProcessor* processor = m_processorManager->GetByProcessorName(processorName, nullptr);
				output = processor->Process(input, context);

				delete context;

				output->m_processorName = processor->GetOutputWriterName();
			}
		}
		//TO-DO: iniciarlizar un ID único para el BuildItem (Asset).
		SerializeAsset(item, output);

		item->m_isBuilt = true;
		return output;
	}

	ContentItem* GameContentBuilder::ImportAsset(BuildItem* item)
	{
		std::string absolutePath = GetAbsolutePath(item->m_buildRequest->m_sourceFileName);
		std::string& importerName = item->m_buildRequest->m_importerName;

		item->m_sourceTimestamp = m_timestampCache.GetTimestamp(absolutePath);
		
		IContentImporter* instance = m_importerManager->GetByImporterName(importerName);
		//
		ContentItem* input = instance->Import(absolutePath);
		//
		if (input != nullptr)
		{
			input->m_importerName = item->m_buildRequest->m_importerName;
		}

		return input;
	}

	void GameContentBuilder::SerializeAsset(BuildItem* item, ContentItem* assetObject)
	{
		//__typeof(assetObject);
		BrunoIO::Path::CreateFolder(BrunoIO::Path::GetDirectoryFromFilePath(GetAbsolutePath(item->m_outputFilename)));
		std::string absolutePath = GetAbsolutePath(item->m_outputFilename);
		m_timestampCache.Remove(absolutePath);

		BrunoIO::FileStream* fileStream = new BrunoIO::FileStream(absolutePath, BrunoIO::FileAccess::Write);
		{
			m_contentCompiler->Compile(fileStream, assetObject, false, m_settings->GetOutputDirectory(), absolutePath);
			fileStream->Close();
		}
		delete fileStream;

		m_rebuiltFiles.push_back(absolutePath);
	}

	void GameContentBuilder::RequestBuild(std::string sourceFilename, std::string assetName, std::string importerName, std::string processorName, OpaqueData* processorParameters)
	{
		BuildRequest* request = new BuildRequest();
		request->m_assetName = assetName;
		request->m_sourceFileName = sourceFilename;
		request->m_importerName = importerName;
		request->m_processorName = processorName;

		request->m_opaqueData = new OpaqueData();
		if (processorParameters != nullptr)
		{
			request->m_opaqueData->addProperties(processorParameters->begin(), processorParameters->end());
		}

		RequestBuild(request);
	}

	BuildItem* GameContentBuilder::RequestBuild(BuildRequest* request)
	{
		request->m_sourceFileName = GetRelativePath(request->m_sourceFileName);

		if (request->m_assetName.size() > 0) {
			if (BrunoIO::Path::IsPathRooted(request->m_assetName) ||
				request->m_assetName.find("..")!= std::string::npos)
			{
				throw ContentPipelineException("Invalid asset name");
			}
			//request->m_assetName = request->m_assetName.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
		}

		if (request->m_importerName.size() == 0)
		{
			//Adivinar el importador por medio de la extension del archivo.
			request->m_importerName = m_importerManager->GetImporterNameByExtension
			(
				BrunoIO::Path::GetFileExtension(request->m_sourceFileName)
			);
		}

		//TO-DO: verificar el "hashcode" de BuildRequest.
		BuildItem* item = m_buildItems->FindItem(request);

		if (((item != nullptr) && (item->m_buildRequest->m_assetName.size() == 0)) && (request->m_assetName.size() > 0))
		{
			RemoveBuildItem(item);
			item = nullptr;
		}

		if (item == nullptr)
		{
			item = new BuildItem();
			item->m_buildRequest = request;
			item->m_outputFilename = ChooseOutputFilename(request);
			m_buildItems->PushBack(item);

			m_buildItemsChanged = true;
		}

		m_buildItems->WantItem(item);
		return item;
	}

	void GameContentBuilder::RemoveBuildItem(BuildItem* item)
	{
		m_buildItems->Remove(item);
		std::string absolutePath = GetAbsolutePath(item->m_outputFilename);

		auto its = std::find(m_rebuiltFiles.begin(), m_rebuiltFiles.end(), item->m_outputFilename);
		if (its != m_rebuiltFiles.end())
		{
			m_rebuiltFiles.erase(its);
		}
		m_timestampCache.Remove(absolutePath);
		BrunoIO::File::Delete(absolutePath);

		for (int i = 0; i < m_buildItems->Size(); i++)
		{
			BuildItem* current = (*m_buildItems)[i];
			if (current->ContainsDependency(item->m_outputFilename) || current->ContainsRequest(item->m_outputFilename))
			{
				current->RemoveDependency(item->m_outputFilename);
				current->RemoveRequest(item->m_outputFilename);
				current->m_sourceTimestamp = std::filesystem::file_time_type::min();
				if (current->m_isBuilt)
				{
					its = find(m_rebuiltFiles.begin(), m_rebuiltFiles.end(), GetAbsolutePath(current->m_outputFilename));
					if (its != m_rebuiltFiles.end())
						m_rebuiltFiles.erase(its);

					current->m_isBuilt = false;
					current->m_isWanted = false;
					m_buildItems->WantItem(current);
				}
			}
		}
	}

	std::string GameContentBuilder::GetAbsolutePath(std::string path)
	{
		return m_settings->GetAbsolutePath(path);
	}

	std::string GameContentBuilder::ChooseOutputFilename(BuildRequest *request)
	{
		std::string intermediateDirectory = m_settings->GetOutputDirectory();
		std::string outputExtension = ".bruno";
		std::string outputFilename;

		if (request->m_assetName.size() == 0)
		{
			if (!BrunoIO::Path::IsPathRooted(request->m_sourceFileName))
			{
				std::string directoryName = BrunoIO::Path::GetDirectoryFromFilePath(request->m_sourceFileName);
				std::string relativePath = GetRelativePath(m_settings->GetIntermediateDirectory());
				if (directoryName.size() > 0 && directoryName.find(relativePath) == 0)
				{
					directoryName = directoryName.substr(relativePath.size());
				}
				if (directoryName.size() > 0) {
					intermediateDirectory = intermediateDirectory + directoryName + '\\';
				}
			}
			std::string assetNameStub = GetAssetNameStub(request->m_sourceFileName);
			int num = 0;
			do {
				std::stringstream ss;
				ss << intermediateDirectory << assetNameStub << '_' << num << outputExtension;

				outputFilename = GetRelativePath(ss.str());

				++num;
			} while (m_buildItems->FindItem(assetNameStub));

			//request->m_assetName = BrunoIO::Path::Combine(BrunoIO::Path::GetDirectoryFromFilePath(request->m_sourceFileName), BrunoIO::Path::GetFilenameWithoutExtension(request->m_sourceFileName));
		}
		else
		{
			std::stringstream ss;
			ss << intermediateDirectory << request->m_assetName << outputExtension;

			outputFilename = GetRelativePath(ss.str());
			BuildItem* buildItem = m_buildItems->FindItem(outputFilename);
			if (buildItem)
			{
				if (buildItem->m_isWanted)
				{
					throw ContentPipelineException("Compiled asset filename conflict");
				}
				RemoveBuildItem(buildItem);
			}
		}

		/*std::string relativePath = GetRelativePath(BrunoIO::Path::Combine(intermediateDirectory, request->m_assetName) + ".trio");

		BuildItem* item = m_buildItems->FindItem(relativePath);

		if (item != nullptr)
		{
			if (item->m_isWanted)
			{
				throw std::exception();
			}
			RemoveBuildItem(item);
		}*/
		return outputFilename;
	}

	std::string GameContentBuilder::GetAssetNameStub(std::string filename)
	{
		std::string text = BrunoIO::Path::GetFilenameWithoutExtension(filename);
		if (text.size() > 32) {
			text = text.substr(0, 32);
		}
		return text;
	}

	std::vector<std::string> GameContentBuilder::GetOutputFiles()
	{
		std::vector<std::string> output;
		for (size_t i = 0; i < m_buildItems->Size(); i++)
		{
			auto item = (*m_buildItems)[i];
			output.push_back(GetAbsolutePath(item->m_outputFilename));
		}
		return output;
	}
}