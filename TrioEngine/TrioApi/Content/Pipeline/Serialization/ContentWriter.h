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
		ContentWriter(ContentCompiler* compiler, TrioIO::Stream* _stream, bool compressContent, std::string rootDirectory, std::string referenceRelocationPath);
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
		const int HeaderSize = 7;
		const char* FileExtension = "estero";

		std::string m_rootDirectory;
		std::string m_referenceRelocationPath;

		ContentCompiler *m_compiler;

		std::map<std::string, int> m_typeTable;
		std::vector<ContentTypeWriter *> m_typeWriters;

		ContentTypeWriter* GetTypeWriter(std::string name, int &typeIndex);

		void WriteFinalOutput();
		void WriteCompressedOutput();
		void WriteUncompressedOutput();
		void WriteHeader();
		void WriteSharedResources();

		TrioIO::MemoryStream* m_headerData;
		TrioIO::MemoryStream* m_contentData;

		TrioIO::Stream* m_finalOutput;

		//std::map<int, int> m_SharedResourceNames;
		std::map<ContentItem*, int> m_sharedResourceNames;
		std::queue<ContentItem*> m_sharedResources;

		std::map<ContentItem*, bool> m_recurseDetector;
	};
}