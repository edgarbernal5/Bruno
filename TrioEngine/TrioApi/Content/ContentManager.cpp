#include "stdafx.h"
#include "ContentManager.h"

#include "IServiceProvider.h"
#include "ContentTypeReaderManager.h"
#include "Graphics/GraphicsDevice.h"
#include "ContentReader.h"

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

	ContentManager::~ContentManager()
	{
	}
	void* ContentManager::LoadInternal(std::string assetName) {
		std::map<std::string, void*>::iterator it = m_LoadedAssets.find(assetName);
		if (it != m_LoadedAssets.end())
		{
			return it->second;
		}

		void* local = ReadAsset<void*>(assetName);
		m_LoadedAssets[assetName] = local;
		return local;
	}
}