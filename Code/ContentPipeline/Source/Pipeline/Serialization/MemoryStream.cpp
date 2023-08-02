#include "MemoryStream.h"

#include <string.h>
#include <exception>

namespace Bruno
{
	MemoryStream::MemoryStream() :
		m_buffer(nullptr),
		m_capacity(0)
	{
		m_length = 0;
		m_position = 0;
	}

	MemoryStream::MemoryStream(uint32_t capacity) :
		m_buffer(nullptr),
		m_capacity(capacity)
	{
		m_length = 0;
		m_position = 0;
	}

	uint8_t* MemoryStream::GetBuffer()
	{
		return m_buffer;
	}

	long MemoryStream::GetLength()
	{
		return (long)(m_length);
	}

	int MemoryStream::ReadByte()
	{
		if (m_position >= m_length)
		{
			return -1;
		}
		return m_buffer[m_position++];
	}

	int MemoryStream::Read(uint8_t* buffer, int count)
	{
		int byteCount = m_length - m_position;
		if (byteCount > count)
		{
			byteCount = count;
		}
		if (byteCount <= 0)
		{
			return 0;
		}
		if (byteCount <= 8)
		{
			int num2 = byteCount;
			while (--num2 >= 0)
			{
				buffer[num2] = m_buffer[m_position + num2];
			}
		}
		else
		{
			memcpy(buffer + m_position, m_buffer, byteCount);
		}
		m_position += byteCount;
		return byteCount;
	}

	void MemoryStream::WriteByte(uint8_t value)
	{
		if (m_position >= m_length)
		{
			uint32_t num = m_position + 1;
			bool flag = m_position > m_length;
			if ((num >= m_capacity) && EnsureCapacity(num))
			{
				flag = false;
			}
			if (flag)
			{
				//Esto no es necesario.
				//memcpy(m_buffer + m_length, 0x00, m_position - m_length);
			}
			m_length = num;
		}
		m_buffer[m_position++] = value;
	}

	void MemoryStream::Write(uint8_t* buffer, int count)
	{
		int num = m_position + count;
		if (num < 0)
		{
			throw std::exception("stream is too long");
		}
		if (num > m_length)
		{
			bool flag = m_position > m_length;
			if ((num > m_capacity) && EnsureCapacity(num))
			{
				flag = false;
			}
			if (flag)
			{
				//Esto no es necesario.
				//memcpy(buffer + m_length, 0x00, num - m_length);
			}
			m_length = num;
		}
		if ((count <= 8) && (buffer != m_buffer))
		{
			int num2 = count;
			while (--num2 >= 0)
			{
				m_buffer[m_position + num2] = buffer[num2];
			}
		}
		else
		{
			memcpy(m_buffer + m_position, buffer, count);
		}
		m_position = num;
	}

	bool MemoryStream::EnsureCapacity(uint32_t value)
	{
		if (value <= m_capacity)
			return false;

		uint32_t num = value;
		if (num < 0x100)
		{
			num = 0x100;
		}
		if (num < (m_capacity * 2))
		{
			num = m_capacity * 2;
		}
		uint8_t* newbuffer = new uint8_t[num];

		if (m_length > 0)
		{
			memcpy(newbuffer, m_buffer, m_length);
		}
		if (m_buffer)
			delete[] m_buffer;

		m_capacity = num;
		m_buffer = newbuffer;
		return true;
	}
}