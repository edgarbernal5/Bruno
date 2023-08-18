#include "brpch.h"
#include "FileStream.h"

namespace Bruno
{
	Bruno::FileStream::FileStream(const std::wstring& filename, FileAccess fileAccess) :
		m_filename(filename),
		m_fileAccess(fileAccess),
		m_fileLength(0)
	{
		if ((fileAccess & FileAccess::Read) != FileAccess::None)
		{
			if ((fileAccess & FileAccess::Write) != FileAccess::None)
				m_stream.open(filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
			else
				m_stream.open(filename.c_str(), std::fstream::in | std::fstream::binary);
		}
		else if ((fileAccess & FileAccess::Write) != FileAccess::None)
			m_stream.open(filename.c_str(), std::fstream::out | std::fstream::binary);

		if (m_stream.good() && (fileAccess & FileAccess::Read) != FileAccess::None)
		{
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
		if ((m_fileAccess & FileAccess::Write) != FileAccess::None)
		{
			return (long)m_stream.tellp();
		}
		return m_fileLength;
	}

	uint64_t FileStream::GetPosition()
	{
		return (uint64_t)m_stream.tellp();
	}

	bool FileStream::IsStreamValid() const
	{
		return m_stream.good();
	}

	void FileStream::SetPosition(uint64_t position)
	{
		m_stream.seekg(position, std::ios::beg);
	}

	bool FileStream::Read(uint8_t* destination, size_t count)
	{
		std::streamsize numBytesRead = 0;

		m_stream.read((char*)destination, count);
		numBytesRead = m_stream.gcount();
		//numBytesRead == count ?
		return true;
	}

	void FileStream::Write(const uint8_t* source, size_t count)
	{
		m_stream.write((char*)source, count);
		
	}
}
