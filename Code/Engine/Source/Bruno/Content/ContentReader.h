#pragma once

//#include <string>
//#include <map>
//#include <vector>

#include "Bruno/Core/Stream.h"

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
		void ReadUInt32(uint32_t& output);
		void ReadString(std::string& output);

	private:
		ContentManager* m_contentManager;
		Stream& m_stream;
		std::wstring m_assetName;

		uint32_t ReadHeader();
	};
}

