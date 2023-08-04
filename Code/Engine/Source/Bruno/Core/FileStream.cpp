#include "brpch.h"
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

		if (m_stream.good() && (fileAccess & FileAccess::Read) != FileAccess::None) {
			m_fileLength = std::filesystem::file_size(filename);
		}
	}

	FileStream::~FileStream()
	{
		Close();
	}

	void FileStream::Close()
	{
		m_stream.close();
	}

	long FileStream::GetLength()
	{
		return m_fileLength;
	}

	uint64_t FileStream::GetPosition()
	{
		return m_stream.tellp();
	}

	bool FileStream::IsStreamValid() const
	{
		return m_stream.good();
	}

	bool FileStream::Read(uint8_t* destination, size_t count)
	{
		std::streamsize numBytesRead = 0;

		m_stream.read((char*)destination, count);
		numBytesRead = m_stream.gcount();

		return true;
	}

	void FileStream::Write(const uint8_t* buffer, size_t count)
	{
		m_stream.write((char*)buffer, sizeof(uint8_t) * count);
		
	}
}
