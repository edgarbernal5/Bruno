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

		char ReadChar();
		int ReadInt32();

	private:
		ContentManager* m_contentManager;
		Stream& m_stream;
		std::wstring m_assetName;
	};
}

