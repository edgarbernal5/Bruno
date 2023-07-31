#include "FileStream.h"

namespace Bruno
{
	Bruno::FileStream::FileStream(const std::wstring& filename, FileAccess fileAccess)
	{
		if ((fileAccess & FileAccess::Read) != FileAccess::None)
			if ((fileAccess & FileAccess::Write) != FileAccess::None)
				m_stream.open(filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
			else
				m_stream.open(filename.c_str(), std::fstream::in | std::fstream::binary);
		else if ((fileAccess & FileAccess::Write) != FileAccess::None)
			m_stream.open(filename.c_str(), std::fstream::out | std::fstream::binary);
	}

	void FileStream::Close()
	{
		m_stream.close();
	}

	int FileStream::Read(uint8_t* buffer, int offset, int count)
	{
		return 0;
	}

	void FileStream::Write(uint8_t* buffer, int offset, int count)
	{

	}
}
