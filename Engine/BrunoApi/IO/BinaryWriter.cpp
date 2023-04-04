#include "stdafx.h"
#include "BinaryWriter.h"

#include "Stream.h"

namespace BrunoIO
{
	BinaryWriter::BinaryWriter(Stream *_stream) 
		: m_stream(nullptr)
	{
		m_stream = _stream;
	}

	void BinaryWriter::WriteInt32(int value)
	{
		m_Buffer[0] = (uint8_t)value;
		m_Buffer[1] = (uint8_t)(value >> 8);
		m_Buffer[2] = (uint8_t)(value >> 0x10);
		m_Buffer[3] = (uint8_t)(value >> 0x18);

		WriteBuffer(m_Buffer, 0, 4);
	}

	void BinaryWriter::WriteChar(char value)
	{
		m_Buffer[0] = (uint8_t)value;

		WriteBuffer(m_Buffer, 0, 1);
	}

	void BinaryWriter::WriteBool(bool value)
	{
		m_Buffer[0] = value ? ((uint8_t)1) : ((uint8_t)0);

		WriteBuffer(m_Buffer, 0, 1);
	}

	void BinaryWriter::WriteSingle(float value)
	{
		uint32_t num = *((uint32_t*)&value);

		m_Buffer[0] = (uint8_t)num;
		m_Buffer[1] = (uint8_t)(num >> 8);
		m_Buffer[2] = (uint8_t)(num >> 0x10);
		m_Buffer[3] = (uint8_t)(num >> 0x18);

		WriteBuffer(m_Buffer, 0, 4);
	}

	void BinaryWriter::WriteDouble(double value)
	{
		uint64_t num = *((uint64_t*)&value);

		m_Buffer[0] = (uint8_t)num;
		m_Buffer[1] = (uint8_t)(num >> 8);
		m_Buffer[2] = (uint8_t)(num >> 0x10);
		m_Buffer[3] = (uint8_t)(num >> 0x18);
		m_Buffer[4] = (uint8_t)(num >> 0x20);
		m_Buffer[5] = (uint8_t)(num >> 40);
		m_Buffer[6] = (uint8_t)(num >> 0x30);
		m_Buffer[7] = (uint8_t)(num >> 0x38);

		WriteBuffer(m_Buffer, 0, 8);
	}

	void BinaryWriter::WriteByte(uint8_t value)
	{
		m_Buffer[0] = value;

		WriteBuffer(m_Buffer, 0, 1);
	}

	void BinaryWriter::WriteShort(uint16_t value)
	{
		uint32_t num = *((uint32_t*)&value);

		m_Buffer[0] = (uint8_t)num;
		m_Buffer[1] = (uint8_t)(num >> 8);

		WriteBuffer(m_Buffer, 0, 2);
	}

	void BinaryWriter::WriteUInt32(uint32_t value)
	{
		m_Buffer[0] = (uint8_t)value;
		m_Buffer[1] = (uint8_t)(value >> 8);
		m_Buffer[2] = (uint8_t)(value >> 0x10);
		m_Buffer[3] = (uint8_t)(value >> 0x18);

		WriteBuffer(m_Buffer, 0, 4);
	}

	void BinaryWriter::WriteBytes(uint8_t *value, size_t size)
	{
		m_stream->Write(value, 0, size);
	}

	void BinaryWriter::WriteString(std::string value)
	{
		WriteUInt32((uint32_t)value.size());
		for (int i = 0; i < value.size(); i++)
		{
			uint8_t vi = *(((uint8_t*) &(value[i])));
			WriteByte(vi);
		}

		//WriteBuffer((uint8_t*)(value.c_str()), 0, value.size());
	}

	void BinaryWriter::Close()
	{
		//m_Stream->close();
	}

	void BinaryWriter::WriteBuffer(uint8_t* buffer, int offset, int length)
	{
		m_stream->Write(buffer, offset, length);
	}
}