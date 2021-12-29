#include "stdafx.h"
#include "BinaryReader.h"

#include "Stream.h"

namespace TrioIO
{
	BinaryReader::BinaryReader(Stream* stream) :
		m_stream(stream),
		m_buffer(nullptr)
	{
		m_buffer = new uint8_t[0x10];
	}

	int BinaryReader::Read()
	{
		return InternalReadOneChar();
	}

	int BinaryReader::InternalReadOneChar()
	{
		long position = 0;
		int num = 0;
		int byteCount = 0;
		uint8_t numbyte;
		if (m_stream->CanSeek())
			position = m_stream->GetPosition();

		while (num == 0)
		{
			byteCount = 1; //1 byte porque estamos usando ascii ext, unicode usaríamos 2
			int num4 = m_stream->ReadByte();
			if (num4 == -1)
			{
				byteCount = 0;
				return -1;
			}
			numbyte = (uint8_t)num4;
			num = num4;
		}
		if (num == 0)
		{
			return -1;
		}
		return numbyte;
	}


	int BinaryReader::Read(uint8_t* buffer, int index, int count)
	{
		return m_stream->Read(buffer, index, count);
	}

	uint8_t BinaryReader::ReadByte()
	{
		int num = m_stream->ReadByte();
		if (num == -1)
		{
			throw std::exception();
		}

		return (uint8_t)num;
	}

	std::vector<uint8_t> BinaryReader::ReadBytes(int count)
	{
		std::vector<uint8_t> buffer(count);
		int offset = 0;
		do
		{
			int num2 = m_stream->Read(&buffer[0], offset, count);
			if (num2 == 0)
			{
				break;
			}
			offset += num2;
			count -= num2;
		} while (count > 0);

		if (offset != count)
		{
			std::vector<uint8_t> dst(offset);
			//Buffer.InternalBlockCopy(buffer, 0, dst, 0, offset);
			memcpy(&dst[0], &buffer[0], offset);

			buffer = dst;
		}
		return buffer;
	}

	void BinaryReader::FillBuffer(int numBytes)
	{
		int offset = 0;
		int num2 = 0;
		if (numBytes == 1)
		{
			num2 = m_stream->ReadByte();
			if (num2 == -1)
			{
				throw std::exception();
			}
			m_buffer[0] = (uint8_t)num2;
		}
		else
		{
			do
			{
				num2 = m_stream->Read(m_buffer, offset, numBytes - offset);
				if (num2 == 0)
				{
					throw std::exception();
				}
				offset += num2;
			} while (offset < numBytes);
		}
	}

	uint16_t BinaryReader::ReadUInt16()
	{
		FillBuffer(2);
		return uint16_t(m_buffer[0]) | uint16_t(m_buffer[1]) << 8;
	}

	uint32_t BinaryReader::ReadUInt32()
	{
		FillBuffer(4);

		return uint32_t(m_buffer[0]) |
			uint32_t(m_buffer[1]) << 8 |
			uint32_t(m_buffer[2]) << 16 |
			uint32_t(m_buffer[3]) << 24;
	}

	uint64_t BinaryReader::ReadUInt64()
	{
		FillBuffer(8);

		return uint64_t(m_buffer[0]) |
			uint64_t(m_buffer[1]) << 8 |
			uint64_t(m_buffer[2]) << 16 |
			uint64_t(m_buffer[3]) << 24 |
			uint64_t(m_buffer[4]) << 32 |
			uint64_t(m_buffer[5]) << 40 |
			uint64_t(m_buffer[6]) << 48 |
			uint64_t(m_buffer[7]) << 56;
	}

	int8_t BinaryReader::ReadSByte()
	{
		return (int8_t)ReadByte();
	}

	int16_t BinaryReader::ReadInt16()
	{
		return (int16_t)ReadUInt16();
	}

	int32_t BinaryReader::ReadInt32()
	{
		return (int32_t)ReadUInt32();
	}

	int64_t BinaryReader::ReadInt64()
	{
		return (int64_t)ReadUInt64();
	}

	float BinaryReader::ReadSingle()
	{
		uint32_t value = ReadUInt32();
		return *(float*)&value;
	}

	double BinaryReader::ReadDouble()
	{
		uint64_t value = ReadUInt64();

		return *(double*)&value;
	}

	bool BinaryReader::ReadBoolean()
	{
		return ReadByte() ? true : false;
	}

	char BinaryReader::ReadChar()
	{
		int num = Read();
		if (num == -1)
		{
			throw std::exception("EOF");
		}
		return (char)num;
	}

	std::string BinaryReader::ReadString()
	{
		uint32_t capacity = ReadUInt32();
		if (capacity == 0) {
			return "";
		}
		//
		////std::vector<uint8_t> strBuffer = ReadBytes(capacity);
		//std::vector<uint8_t> strBuffer(capacity);
		//int byteCount = m_Stream->Read(&strBuffer[0], 0, capacity);

		//std::string str((char*)(&strBuffer[0]), (char*)(&strBuffer[0 + capacity]));
		std::vector<uint8_t> strBuffer(capacity);
		//strBuffer.push_back(0);

		std::string retstr = "";
		int num = 0;
		do
		{
			int count = ((capacity - num) > 0x80) ? 0x80 : (capacity - num);
			int byteCount = m_stream->Read(strBuffer.data(), 0, count);
			if (byteCount == 0)
			{
				//error. End of file
			}
			if ((num == 0) && (byteCount == capacity))
			{
				strBuffer.push_back(0);
				retstr = std::string((char*)(strBuffer.data()), (char*)(strBuffer.data() + capacity));

				return retstr;
			}
			retstr.insert(retstr.end(), (char*)(strBuffer.data()), (char*)(strBuffer.data() + count));
			num += byteCount;
		} while (num < capacity);

		return retstr;
	}
}