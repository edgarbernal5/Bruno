#include "stdafx.h"
#include "ContentManager.h"

#include "IServiceProvider.h"
#include "ContentTypeReaderManager.h"
#include "Graphics/GraphicsDevice.h"
#include "ContentReader.h"

#include "IO/Path.h"
#include "Vfs/IStorage.h"
#include "Vfs/FileSystemStorage.h"

namespace BrunoEngine
{
	ContentManager::ContentManager(IServiceProvider* serviceProvider) :
		m_serviceProvider(serviceProvider), 
		m_device(nullptr),
		m_storage(nullptr)
	{
		ContentTypeReaderManager::RegisterStandardTypes();

		m_storage = new BrunoIO::FileSystemStorage("");
	}

	ContentManager::ContentManager(GraphicsDevice* device) :
		m_serviceProvider(nullptr), 
		m_device(device),
		m_storage(nullptr)
	{
		ContentTypeReaderManager::RegisterStandardTypes();
		m_storage = new BrunoIO::FileSystemStorage("");
	}

	ContentManager::ContentManager(GraphicsDevice* device, BrunoIO::IStorage* storage) :
		m_serviceProvider(nullptr), 
		m_device(device),
		m_storage(storage)
	{
		ContentTypeReaderManager::RegisterStandardTypes();
	}

	ContentManager::ContentManager(GraphicsDevice* device, std::string rootDirectory) :
		m_serviceProvider(nullptr), m_device(device),
		m_storage(nullptr)
	{
		ContentTypeReaderManager::RegisterStandardTypes();
		m_storage = new BrunoIO::FileSystemStorage(rootDirectory);
	}

	ContentManager::~ContentManager()
	{
	}

	void* ContentManager::LoadInternal(std::string assetName)
	{
		auto it = m_loadedAssets.find(assetName);
		if (it != m_loadedAssets.end())
		{
			return it->second;
		}

		uint8_t* local = ReadAsset(assetName);
		m_loadedAssets[assetName] = local;
		return local;
	}

	uint8_t* ContentManager::ReadAsset(std::string assetName)
	{
		uint8_t* local = nullptr;

		auto stream = OpenStream(assetName);
		{
			ContentReader* reader = ContentReader::Create(this, stream, assetName);
			local = reader->ReadAsset<uint8_t>();

			delete reader;
		}
		stream->Close();
		delete stream;

		return local;
	}

	BrunoIO::Stream* ContentManager::OpenStream(std::string assetName)
	{
		/*std::string rootDirectory = m_rootDirectory;
		if (rootDirectory.size() == 0) rootDirectory = ".";

		std::string fullDirectory = BrunoIO::Path::GetFullDirectory(rootDirectory + BrunoIO::Path::DirectorySeparator);*/
		std::string path = assetName + ".bruno";
		
		//BrunoIO::FileStream* fileStream = new BrunoIO::FileStream(path, BrunoIO::FileAccess::Read);
		
		return m_storage->OpenFile(path);
	}

	/*
	string GetCleanPath(string path)
{
	path = path.Replace('/', '\\');
	path = path.Replace("\\.\\", "\\");
	while (path.StartsWith(".\\"))
	{
		path = path.Substring(".\\".Length);
	}
	while (path.EndsWith("\\."))
	{
		if (path.Length > "\\.".Length)
		{
			path = path.Substring(0, path.Length - "\\.".Length);
		}
		else
		{
			path = "\\";
		}
	}
	for (int i = 1; i < path.Length; i = TitleContainer.CollapseParentDirectory(ref path, i, "\\..\\".Length))
	{
		i = path.IndexOf("\\..\\", i);
		if (i < 0)
		{
			break;
		}
	}
	if (path.EndsWith("\\.."))
	{
		int i = path.Length - "\\..".Length;
		if (i > 0)
		{
			TitleContainer.CollapseParentDirectory(ref path, i, "\\..".Length);
		}
	}
	if (path == ".")
	{
		path = string.Empty;
	}
	return path;
}

*/
}