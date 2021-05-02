#include "stdafx.h"
#include "ContentManager.h"

#include "IServiceProvider.h"
#include "ContentTypeReaderManager.h"
#include "Graphics/GraphicsDevice.h"
#include "ContentReader.h"

#include "IO/Path.h"

namespace TrioEngine
{
	ContentManager::ContentManager(IServiceProvider* serviceProvider) 
		: m_serviceProvider(serviceProvider), m_device(nullptr)
	{
		ContentTypeReaderManager::RegisterStandardTypes();
	}

	ContentManager::ContentManager(GraphicsDevice* device) 
		: m_serviceProvider(nullptr), m_device(device)
	{
		ContentTypeReaderManager::RegisterStandardTypes();
	}

	ContentManager::ContentManager(GraphicsDevice* device, std::string rootDirectory)
		: m_serviceProvider(nullptr), m_device(device), m_rootDirectory(rootDirectory)
	{
		ContentTypeReaderManager::RegisterStandardTypes();
	}

	ContentManager::~ContentManager()
	{
	}

	std::string ContentManager::GetRootDirectory()
	{
		return m_rootDirectory;
	}

	void ContentManager::SetRootDirectory(std::string directory)
	{
		m_rootDirectory = directory;
	}

	void* ContentManager::LoadInternal(std::string assetName) {
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

	TrioIO::Stream* ContentManager::OpenStream(std::string assetName)
	{
		//ou = TrioIO::Path::GetFullDirectory(m_rootDirectory + TrioIO::Path::DirectorySeparator);
		std::string rootDirectory = m_rootDirectory;
		if (rootDirectory.size() == 0) rootDirectory = ".";

		std::string fullDirectory = TrioIO::Path::GetFullDirectory(rootDirectory + TrioIO::Path::DirectorySeparator);
		std::string path = TrioIO::Path::Combine(fullDirectory, assetName + ".estero");
		
		TrioIO::FileStream* fileStream = new TrioIO::FileStream(path, TrioIO::FileAccess::Read);

		return fileStream;
	}
}