#include "stdafx.h"
#include "FileStream.h"

namespace TrioIO
{
	FileStream::FileStream(std::string filename, FileAccess access) :
		Stream(),
		m_Access(access),
		m_FileName(filename)
	{
		if ((m_Access & FileAccess::Read) != FileAccess::None)
			if ((m_Access & FileAccess::Write) != FileAccess::None)
				m_Stream.open(m_FileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
			else
				m_Stream.open(m_FileName.c_str(), std::fstream::in | std::fstream::binary);
		else if ((m_Access & FileAccess::Write) != FileAccess::None)
			m_Stream.open(m_FileName.c_str(), std::fstream::out | std::fstream::binary);

		m_canSeek = true;
		m_pos = 0;
		m_readPos = 0;
		m_readLen = 0;
		m_writePos = 0;
		m_appendStart = -1;
		m_bufferSize = 0x1000;

		m_canRead = (m_Access & FileAccess::Read) != FileAccess::None;

		m_buffer = nullptr;

		m_Stream.seekg(0, m_Stream.end);
		m_nativeLength = m_Stream.tellg();
		m_Stream.seekg(0, m_Stream.beg);

		m_validStream = true;
	}

	FileStream::~FileStream()
	{
	}

	bool FileStream::CanRead()
	{
		return m_canRead;
	}

	bool FileStream::CanSeek()
	{
		return m_canSeek;
	}

	long FileStream::GetPosition()
	{
		return (m_pos + ((m_readPos - m_readLen) + m_writePos));
	}

	void FileStream::Close()
	{
		if (m_validStream)
		{
			if (m_writePos > 0)
			{
				FlushWrite(false);
			}
			m_Stream.close();
			m_validStream = false;
		}
	}

	long FileStream::GetLength()
	{
		return m_nativeLength;

	}

	int FileStream::ReadByte()
	{
		if ((m_readLen == 0) && !CanRead())
			throw std::exception();

		if (m_readPos == m_readLen)
		{
			if (m_writePos > 0)
			{
				FlushWrite(false);
			}
			if (m_buffer == nullptr)
			{
				m_buffer = new uint8_t[m_bufferSize];
			}
			m_readLen = ReadCore(m_buffer, 0, m_bufferSize);
			m_readPos = 0;
		}
		if (m_readPos == m_readLen)
		{
			return -1;
		}
		int num = m_buffer[m_readPos];
		m_readPos++;
		return num;
	}

	int FileStream::Read(uint8_t* buffer, int offset, int count)
	{
		bool flag = false;
		int num = m_readLen - m_readPos;
		if (num == 0)
		{
			if (!CanRead())
			{
				throw std::exception("unreadable stream");
			}
			if (m_writePos > 0)
			{
				FlushWrite(false);
			}
			if (!CanSeek() || (count >= m_bufferSize))
			{
				num = ReadCore(buffer, offset, count);
				m_readPos = 0;
				m_readLen = 0;
				return num;
			}
			if (m_buffer == nullptr)
			{
				m_buffer = new uint8_t[m_bufferSize];
			}
			num = ReadCore(m_buffer, 0, m_bufferSize);
			if (num == 0)
			{
				return 0;
			}
			flag = num < m_bufferSize;
			m_readPos = 0;
			m_readLen = num;
		}
		if (num > count)
		{
			num = count;
		}
		memcpy(buffer + offset, m_buffer + m_readPos, num);
		m_readPos += num;
		if (num < count && !flag)
		{
			//revisar el tama;o del buffer
			int num2 = ReadCore(buffer, offset + num, count - num);
			num += num2;
			m_readPos = 0;
			m_readLen = 0;
		}
		return num;

		/*
		for (int i = 0; i < count; i++)
		{
			m_Stream >> buffer[i];
		}
		return 0;
		*/
	}

	int FileStream::ReadCore(uint8_t* buffer, int offset, int count)
	{
		long long numBytesRead = 0;

		m_Stream.read((char*)(buffer + offset), count);
		numBytesRead = m_Stream.gcount();

		//int numBytesRead = 0;
		//
		//while (numBytesRead < count)
		//{
		//	if (m_Stream.eof())
		//		break;
		//	char aux;

		//	m_Stream >> aux;
		//	//m_Stream >> buffer[offset + numBytesRead];
		//	buffer[offset + numBytesRead] = (uint8_t)aux;
		//	++numBytesRead;
		//}

		m_pos += numBytesRead;
		return numBytesRead;
	}

	void FileStream::FlushWrite(bool calledFromFinalizer)
	{
		WriteCore(m_buffer, 0, m_writePos);
		m_writePos = 0;
	}


	long FileStream::Seek(long offset, SeekOrigin origin)
	{
		return 0;
	}

	void FileStream::WriteCore(uint8_t* buffer, int offset, int count)
	{
		m_Stream.write((char*)(buffer + offset), count);
		if (m_Stream.good())
			m_pos += count;
	}

	void FileStream::Write(uint8_t* buffer, int offset, int count)
	{
		//TO-DO: ver .cs
		//m_Stream.write((char*)(buffer + offset), count);

		if (m_writePos == 0)
		{
			if (m_readPos < m_readLen)
			{
				//FlushRead();
			}
			m_readPos = 0;
			m_readLen = 0;
		}
		if (m_writePos > 0)
		{
			int num = m_bufferSize - m_writePos;
			if (num > 0)
			{
				if (num > count)
				{
					num = count;
				}
				//Buffer.InternalBlockCopy(array, offset, this._buffer, this._writePos, num);
				memcpy(m_buffer + m_writePos, buffer + offset, num);
				m_writePos += num;
				if (count == num)
				{
					return;
				}
				offset += num;
				count -= num;
			}
			WriteCore(m_buffer, 0, m_writePos);
			m_writePos = 0;
		}

		if (count >= m_bufferSize)
		{
			WriteCore(buffer, offset, count);
		}
		else if (count != 0)
		{
			if (m_buffer == nullptr)
			{
				m_buffer = new byte[m_bufferSize];
			}
			//Buffer.InternalBlockCopy(array, offset, this._buffer, this._writePos, count);
			memcpy(m_buffer + m_writePos, buffer + offset, count);
			m_writePos = count;
		}


		//for (int i = 0; i < count; i++)
		//{
		//	m_Stream << buffer[i + offset];
		//}

	}
}