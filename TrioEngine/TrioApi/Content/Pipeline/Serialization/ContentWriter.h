#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ExternalReference.h"
#include "IO/BinaryWriter.h"
#include <map>
#include <string>
#include <vector>
#include <queue>

namespace TrioIO
{
	class Stream;
	class MemoryStream;
}

namespace TrioEngine
{
	class ContentCompiler;
	class ContentItem;
	class ContentTypeWriter;

	class ContentWriter : public TrioIO::BinaryWriter
	{
	public:
		ContentWriter(ContentCompiler* compiler, TrioIO::Stream* _stream, bool compressContent);
		~ContentWriter();

		void Write(Matrix value);
		void Write(Vector2 value);
		void Write(Vector3 value);
		void Write(Vector4 value);

		//template <ContentItem* T>
		void WriteObject(ContentItem* value);
		void WriteSharedResource(ContentItem* resource);

		void WriteExternalReference(ExternalReference& reference);

		void BeginWrite();
		void FlushOutput();
	private:
		ContentCompiler *m_Compiler;

		std::map<std::string, int> m_TypeTable;
		std::vector<ContentTypeWriter *> m_TypeWriters;

		ContentTypeWriter* GetTypeWriter(std::string name, int &typeIndex);

		void WriteFinalOutput();
		void WriteCompressedOutput();
		void WriteUncompressedOutput();
		void WriteHeader();
		void WriteSharedResources();

		TrioIO::MemoryStream* m_HeaderData;
		TrioIO::MemoryStream* m_ContentData;

		TrioIO::Stream* m_FinalOutput;

		//std::map<int, int> m_SharedResourceNames;
		std::map<ContentItem*, int> m_SharedResourceNames;
		std::queue<ContentItem*> m_SharedResources;

		std::map<ContentItem*, bool> m_recurseDetector;
	};
}