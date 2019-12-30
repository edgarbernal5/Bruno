#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <vector>

#include "IO/FileStream.h"
//#include "ContentReader.h"

namespace TrioEngine
{
	class GraphicsDevice;
	class IServiceProvider;
	//class ContentReader;

	class TRIO_API_EXPORT ContentManager
	{
	public:
		ContentManager(GraphicsDevice* device);
		ContentManager(IServiceProvider* serviceProvider);
		~ContentManager();

		template <class T>
		T* Load(std::string assetName);

		inline IServiceProvider* GetServiceProvider()
		{
			return m_serviceProvider;
		}
		inline GraphicsDevice* GetGraphicsDevice()
		{
			return m_device;
		}

		friend class ContentReader;
	private:
		std::map<std::string, void*> m_LoadedAssets;

		std::string m_RootDirectory;

		void* LoadInternal(std::string assetName);

		template <class T>
		T* ReadAsset(std::string assetName);

		IServiceProvider* m_serviceProvider;
		GraphicsDevice* m_device;
	};


	template <class T>
	T* ContentManager::Load(std::string assetName)
	{
		std::map<std::string, void*>::iterator it = m_LoadedAssets.find(assetName);
		if (it != m_LoadedAssets.end())
		{
			T* casteo = static_cast<T*>(it->second);
			if (casteo != nullptr)
				return casteo;

			return nullptr;
		}

		T* local = ReadAsset<T>(assetName);
		m_LoadedAssets[assetName] = local;
		return local;
	}

	template <class T>
	T* ContentManager::ReadAsset(std::string assetName)
	{
		T* local = nullptr;

		TrioIO::FileStream* fileStream = new TrioIO::FileStream(assetName, TrioIO::FileAccess::Read);
		{
			ContentReader* reader = ContentReader::Create(this, fileStream, assetName);
			local = reader->ReadAsset<T>();

			delete reader;
		}
		fileStream->Close();
		delete fileStream;

		return local;
	}
}