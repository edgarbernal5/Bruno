#pragma once

#include <string>
#include <vector>

#include "Bruno/Core/Stream.h"
#include "Bruno/Math/Math.h"
#include "ContentTypeReader.h"
#include <functional>

namespace Bruno
{
	class ContentManager;
	class MemoryStream;

	class ContentReader
	{
	public:
		ContentReader(ContentManager* contentManager, Stream& stream, const std::wstring& assetName);
		~ContentReader();

		std::shared_ptr<RTTI> ReadAsset();

		void ReadChar(char& output);
		std::shared_ptr<RTTI> ReadExternalReference();
		void ReadInt32(int32_t &output);
		void ReadInt64(int64_t &output);
		void ReadUInt8(uint8_t& output);
		void ReadUInt32(uint32_t& output);
		void ReadUInt64(uint64_t& output);
		void ReadString(std::string& output);
		void ReadWString(std::wstring& output);
		void ReadBytes(std::vector<uint8_t>& output);
		void ReadSharedResources(uint32_t sharedResourceCount);
		void ReadVector2(Math::Vector2& output);
		void ReadVector3(Math::Vector3& output);
		void ReadVector4(Math::Vector4& output);

	private:
		ContentManager* m_contentManager;
		Stream& m_stream;
		Stream* m_currentStream;
		MemoryStream* m_decompressedStream;

		std::wstring m_assetName;
		std::vector<AbstractContentTypeReader*> m_readers;
		std::vector<std::vector<std::function<void(std::shared_ptr<RTTI>)> > > m_sharedResourceFixups;

		void ReadSharedResource(std::function<void(std::shared_ptr<RTTI>)> action);
		uint32_t ReadHeader();
		std::shared_ptr<RTTI> ReadObject();
	};
}

