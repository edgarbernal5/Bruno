#pragma once

#include <string>
#include <vector>

#include "Bruno/Core/Stream.h"
#include "ContentTypeReader.h"

namespace Bruno
{
	class ContentManager;

	class ContentReader
	{
	public:
		ContentReader(ContentManager* contentManager, Stream& stream, const std::wstring& assetName);

		std::shared_ptr<RTTI> ReadAsset();

		void ReadChar(char& output);
		void ReadInt32(int32_t &output);
		void ReadInt64(int64_t &output);
		void ReadUInt8(uint8_t& output);
		void ReadUInt32(uint32_t& output);
		void ReadUInt64(uint64_t& output);
		void ReadString(std::string& output);
		void ReadBytes(std::vector<uint8_t>& output);

	private:
		ContentManager* m_contentManager;
		Stream& m_stream;
		std::wstring m_assetName;
		std::vector<AbstractContentTypeReader*> m_readers;

		uint32_t ReadHeader();
		std::shared_ptr<RTTI> ReadObject();
	};
}

