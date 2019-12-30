#pragma once

#include "TrioApiRequisites.h"

#include <functional>
#include <string>
#include <vector>
#include <stdint.h>

#include "IO/BinaryReader.h"
#include "Content/Pipeline/ContentItem.h"
//#include "ContentManager.h"
#include "Content/ContentTypeReader.h"
//#include "Content/ContentTypeReaderManager.h"

namespace TrioIO
{
	class Stream;
}

namespace TrioEngine
{
	//class ContentTypeReader;
	class ContentManager;

	class TRIO_API_EXPORT ContentReader : public TrioIO::BinaryReader
	{
	public:
		ContentReader(ContentManager* contentManager, TrioIO::Stream *stream, std::string assetName);
		~ContentReader();

		static ContentReader* Create(ContentManager* contentManager, TrioIO::Stream *stream, std::string assetName);

		Vector2 ReadVector2();
		Vector3 ReadVector3();
		Vector4 ReadVector4();

		Matrix ReadMatrix();

		template <class T>
		T* ReadAsset();

		template <class T>
		T* ReadObject();

		void* ReadExternalReference();

		void ReadSharedResource(std::function<void(ContentItem*)> action);
		std::vector<uint8_t> ReadByteBuffer(int size);

		ContentManager* GetContentManager();
	private:
		ContentManager* m_contentManager;
		std::string m_AssetName;

		TrioIO::Stream* m_Stream;

		static void PrepareStream(TrioIO::Stream *input);
		uint32_t ReadHeader();

		std::vector<class ContentTypeReader*> m_TypeReaders;

		template <class T>
		T* ReadObjectInternal(ContentItem* existingInstance);

		void ReadSharedResources(uint32_t sharedResourceCount);

		std::vector<std::vector<std::function<void(ContentItem*)> > > m_sharedResourceFixups;
	};


	template <class T>
	T* ContentReader::ReadAsset()
	{
		T* local = nullptr;
		uint32_t sharedResourceCount = ReadHeader();
		local = ReadObject<T>();
		ReadSharedResources(sharedResourceCount);

		return local;
	}


	template <class T>
	T* ContentReader::ReadObject()
	{
		return ReadObjectInternal<T>(nullptr);
	}

	template <class T>
	T* ContentReader::ReadObjectInternal(ContentItem* existingInstance)
	{
		int index = BinaryReader::ReadInt32();
		if (index == 0)
			return nullptr;

		index--;
		if (index >= m_TypeReaders.size())
		{
			throw std::exception("Bad xnb");
		}

		return (T*)m_TypeReaders[index]->Read(this);
	}
}