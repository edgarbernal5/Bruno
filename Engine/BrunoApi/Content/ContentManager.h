#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <vector>

#include "IO/FileStream.h"
//#include "ContentReader.h"
namespace BrunoIO
{
	class IStorage;
}

namespace BrunoEngine
{
	class GraphicsDevice;
	class IServiceProvider;
	//class ContentReader;

	class BRUNO_API_EXPORT ContentManager
	{
	public:
		ContentManager(GraphicsDevice* device);
		ContentManager(GraphicsDevice* device, BrunoIO::IStorage *storage);
		ContentManager(GraphicsDevice* device, std::string rootDirectory);
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
		BrunoIO::IStorage* m_storage;
		std::map<std::string, void*> m_loadedAssets;


		void* LoadInternal(std::string assetName);

		uint8_t* ReadAsset(std::string assetName);

		IServiceProvider* m_serviceProvider;
		GraphicsDevice* m_device;

	protected:
		BrunoIO::Stream* OpenStream(std::string assetName);
	};

	template <class T>
	T* ContentManager::Load(std::string assetName)
	{
		std::map<std::string, void*>::iterator it = m_loadedAssets.find(assetName);
		if (it != m_loadedAssets.end())
		{
			T* casteo = reinterpret_cast<T*>(it->second);
			if (casteo != nullptr)
				return casteo;

			return nullptr;
		}

		T* local = reinterpret_cast<T*>(ReadAsset(assetName));
		m_loadedAssets[assetName] = local;
		return local;
	}
}