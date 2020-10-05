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

		/*template <class T>
		T* ReadExternalReference();*/

		void* ReadExternalReference();

		void ReadSharedResource(std::function<void(ContentItem*)> action);
		std::vector<uint8_t> ReadByteBuffer(int size);

		ContentManager* GetContentManager();
	private:
		static const int HeaderSize = 7;

		ContentManager* m_contentManager;
		std::string m_assetName;

		TrioIO::Stream* m_stream;
		std::vector<class ContentTypeReader*> m_typeReaders;
		std::vector<std::vector<std::function<void(ContentItem*)> > > m_sharedResourceFixups;

		static void PrepareStream(TrioIO::Stream *input);
		uint32_t ReadHeader();

		template <class T>
		T* ReadObjectInternal(ContentItem* existingInstance);

		void ReadSharedResources(uint32_t sharedResourceCount);
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
		if (index >= m_typeReaders.size())
		{
			throw std::exception("Bad estero type readers");
		}

		return reinterpret_cast<T*>(m_typeReaders[index]->Read(this));
	}

	//template <class T>
	//T* ContentReader::ReadExternalReference()
	//{
	//	std::string cleanPath = ReadString();
	//	if (cleanPath.size() == 0)
	//		return nullptr;

	//	return m_contentManager->Load<T>(cleanPath);
	//}
}