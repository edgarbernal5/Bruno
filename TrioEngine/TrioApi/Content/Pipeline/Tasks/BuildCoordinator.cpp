#include "stdafx.h"
#include "BuildCoordinator.h"

#include "IO/FileStream.h"
#include "IO/File.h"

#include "Content/Pipeline/Serialization/ContentCompiler.h"
#include "Content/Pipeline/Importers/ImporterManager.h"
#include "Content/Pipeline/Importers/IContentImporter.h"
#include "Content/Pipeline/Processors/BasicContentProcessorContext.h"
#include "Content/Pipeline/Processors/ProcessorManager.h"
#include "Content/Pipeline/Processors/IContentProcessor.h"
#include "Content/Pipeline/Tasks/BuildItem.h"
#include "Content/Pipeline/Tasks/BuildItemCollection.h"
#include "Content/Pipeline/Tasks/BuildRequest.h"

namespace TrioEngine
{
	BuildCoordinator::BuildCoordinator(BuildCoordinatorSettings settings, TimestampCache* timestampCache) :
		m_importerManager(nullptr), m_processorManager(nullptr), m_BuildItems(nullptr), m_BuildItemsChanged(false),
		m_Settings(settings)
	{
		m_Settings.InitializePaths();

		m_importerManager = new ImporterManager();
		m_processorManager = new ProcessorManager();

		m_ContentCompiler = new ContentCompiler();

		m_BuildItems = new BuildItemCollection();

		if (timestampCache)
		{
			m_timestampCache = *timestampCache;
		}
	}

	BuildCoordinator::~BuildCoordinator()
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
		if (m_ContentCompiler != nullptr)
		{
			delete m_ContentCompiler;
			m_ContentCompiler = nullptr;
		}
		if (m_BuildItems != nullptr)
		{
			delete m_BuildItems;
			m_BuildItems = nullptr;
		}
	}

	void BuildCoordinator::RunTheBuild()
	{
		BuildItem* item = nullptr;
		m_BuildItems->ReverseWantedItems(0);

		while ((item = m_BuildItems->PopWantedItem()) != nullptr)
		{
			if (!item->m_IsBuilt)
			{
				int wantedItemsCount = m_BuildItems->WantedItemsCount();

				BuildAsset(item);

				//for item.Requests
				//wantitem

				m_BuildItems->ReverseWantedItems(wantedItemsCount);
			}
		}
	}

	ContentItem* BuildCoordinator::BuildAsset(BuildItem* item)
	{
		if (item->m_IsBuilt)
		{
			return nullptr;
		}

		m_BuildItemsChanged = true;
		item->m_Dependencies.clear();
		item->m_ExtraOutputFiles.clear();

		ContentItem* input = ImportAsset(item);

		ContentItem* output = input;
		if (input != nullptr)
		{
			std::string& processorName = item->m_BuildRequest->m_ProcessorName;

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

		item->m_IsBuilt = true;
		return output;
	}

	ContentItem* BuildCoordinator::ImportAsset(BuildItem* item)
	{
		std::string absolutePath = GetAbsolutePath(item->m_BuildRequest->m_SourceFileName);
		std::string& importerName = item->m_BuildRequest->m_ImporterName;

		item->m_SourceTimestamp = m_timestampCache.GetTimestamp(absolutePath);
		
		IContentImporter* instance = m_importerManager->GetByImporterName(importerName);
		ContentItem* input = instance->Import(absolutePath);

		if (input != nullptr)
		{
			input->m_importerName = item->m_BuildRequest->m_ImporterName;
		}

		return input;
	}

	void BuildCoordinator::SerializeAsset(BuildItem* item, ContentItem* assetObject)
	{
		//__typeof(assetObject);
		TrioIO::Path::CreateFolder(TrioIO::Path::GetDirectoryFromFilePath(GetAbsolutePath(item->OutputFilename)));
		std::string absolutePath = GetAbsolutePath(item->OutputFilename);
		m_timestampCache.Remove(absolutePath);

		TrioIO::FileStream* fileStream = new TrioIO::FileStream(absolutePath, TrioIO::FileAccess::Write);
		{
			m_ContentCompiler->Compile(fileStream, assetObject, false);
			fileStream->Close();
		}
		delete fileStream;

		m_RebuiltFiles.push_back(absolutePath);
	}

	void BuildCoordinator::RequestBuild(std::string sourceFilename, std::string assetName, std::string importerName, std::string processorName, OpaqueData* processorParameters)
	{
		BuildRequest* request = new BuildRequest();
		request->m_AssetName = assetName;
		request->m_SourceFileName = sourceFilename;
		request->m_ImporterName = importerName;
		request->m_ProcessorName = processorName;

		if (processorParameters != nullptr)
		{
			request->m_OpaqueData = new OpaqueData(*processorParameters);
		}
		else
		{
			request->m_OpaqueData = new OpaqueData();
		}

		RequestBuild(request);
	}

	BuildItem* BuildCoordinator::RequestBuild(BuildRequest* request)
	{
		request->m_SourceFileName = GetRelativePath(request->m_SourceFileName);
		if (request->m_ImporterName.size() == 0)
		{
			//Adivinar el importador por medio de la extension del archivo.
			request->m_ImporterName = m_importerManager->GetImporterNameByExtension
			(
				TrioIO::Path::GetFileExtension(request->m_SourceFileName)
			);
		}

		//TO-DO: verificar el "hashcode" de BuildRequest.
		BuildItem* item = m_BuildItems->FindItem(request);

		if (((item != nullptr) && (item->m_BuildRequest->m_AssetName.size() == 0)) && (request->m_AssetName.size() > 0))
		{
			RemoveBuildItem(item);
			item = nullptr;
		}

		if (item == nullptr)
		{
			item = new BuildItem();
			item->m_BuildRequest = request;
			item->OutputFilename = ChooseOutputFilename(request);
			m_BuildItems->PushBack(item);

			m_BuildItemsChanged = true;
		}

		m_BuildItems->WantItem(item);
		return item;
	}

	void BuildCoordinator::RemoveBuildItem(BuildItem* item)
	{
		m_BuildItems->Remove(item);
		std::string absolutePath = GetAbsolutePath(item->OutputFilename);

		std::vector<std::string>::iterator its = find(m_RebuiltFiles.begin(), m_RebuiltFiles.end(), item->OutputFilename);
		if (its != m_RebuiltFiles.end())
		{
			m_RebuiltFiles.erase(its);
		}
		//timestampCache.remove
		TrioIO::File::Delete(absolutePath);

		for (int i = 0; i < m_BuildItems->Size(); i++)
		{
			BuildItem* item2 = (*m_BuildItems)[i];
			its = find(item2->m_Requests.begin(), item2->m_Requests.end(), item2->OutputFilename);
			if (its != item2->m_Requests.end())
			{
				item2->m_Requests.erase(its);
				//item2->m_SourceTime = 
				if (item2->m_IsBuilt)
				{
					its = find(m_RebuiltFiles.begin(), m_RebuiltFiles.end(), GetAbsolutePath(item2->OutputFilename));
					if (its != m_RebuiltFiles.end())
						m_RebuiltFiles.erase(its);

					item2->m_IsBuilt = false;
					item2->m_IsWanted = false;
					m_BuildItems->WantItem(item2);
				}
			}
		}
	}

	std::string BuildCoordinator::GetAbsolutePath(std::string path)
	{
		return m_Settings.GetAbsolutePath(path);
	}

	std::string BuildCoordinator::ChooseOutputFilename(BuildRequest *request)
	{
		std::string intermediateDirectory = m_Settings.OutputDirectory;

		if (request->m_AssetName.size() == 0)
		{
			if (!TrioIO::Path::IsPathRooted(request->m_SourceFileName))
			{
				std::string directoryName = TrioIO::Path::GetDirectoryFromFilePath(request->m_SourceFileName);
				std::string str5 = GetRelativePath(m_Settings.IntermediateDirectory);
			}
			request->m_AssetName = TrioIO::Path::Combine(TrioIO::Path::GetDirectoryFromFilePath(request->m_SourceFileName), TrioIO::Path::GetFilenameWithoutExtension(request->m_SourceFileName));
		}

		std::string relativePath = GetRelativePath(TrioIO::Path::Combine(intermediateDirectory, request->m_AssetName) + ".trio");

		BuildItem* item = m_BuildItems->FindItem(relativePath);

		if (item != nullptr)
		{
			if (item->m_IsWanted)
			{
				throw std::exception();
			}
			RemoveBuildItem(item);
		}
		return relativePath;
	}
}