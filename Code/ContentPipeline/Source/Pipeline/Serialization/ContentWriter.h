#pragma once

#include <string>
#include <map>
#include <queue>
#include <vector>

#include <Bruno/Core/MemoryStream.h>
#include "Pipeline/AbstractProcessor.h"
#include "Pipeline/ExternalReferenceContentItem.h"

#include <Bruno/Math/Math.h>

namespace Bruno
{
	class ContentCompiler;
	class AbstractContentTypeWriter;

	class ContentWriter
	{
	public:
		ContentWriter(ContentCompiler* compiler, Stream& stream, bool compressContent, const std::wstring& rootDirectory, const std::wstring& referenceRelocationPath);

		void FlushOutput();

		void WriteBytes(const std::vector<uint8_t>& buffer);
		void WriteChar(char value);
		void WriteExternalReference(const ExternalReferenceContentItem& reference);
		void WriteInt32(int32_t value);
		void WriteInt64(int64_t value);
		void WriteUInt8(uint8_t value);
		void WriteUInt32(uint32_t value);
		void WriteUInt64(uint64_t value);
		void WriteObject(const ContentItem& object);
		void WriteString(const std::string& value);
		void WriteWString(const std::wstring& value);
		void WriteSharedResource(const ContentItem* resource);
		void WriteSharedResources();
		void WriteVector2(const Math::Vector2& vector2);
		void WriteVector3(const Math::Vector3& vector3);
		void WriteVector4(const Math::Vector4& vector4);

	private:
		const wchar_t* FileExtension = L".bruno";
		bool m_compressContent;
		std::wstring m_referenceRelocationPath;
		Stream& m_finalOutputStream;
		MemoryStream m_headerDataStream;
		MemoryStream m_contentDataStream;

		Stream* m_currentStream;

		std::map<RTTI::IdType, int> m_writersIndexTable;
		std::vector<AbstractContentTypeWriter*> m_writers;
		std::map<const ContentItem*, uint32_t> m_sharedResourcesIndexTable;
		std::queue<const ContentItem*> m_sharedResources;

		AbstractContentTypeWriter* GetTypeWriter(RTTI::IdType writerTypeId, int& typeIndex);
		void WriteHeader();
		void WriteFinalOutput();
	};
}

