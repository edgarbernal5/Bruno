#include "stdafx.h"
#include "MemoryStream.h"

namespace TrioIO
{
	MemoryStream::MemoryStream() : m_buffer(nullptr)
	{
		m_capacity = 0;
		Init();
	}

	MemoryStream::MemoryStream(int capacity) : m_buffer(nullptr)
	{
		m_capacity = capacity;
		Init();
	}

	void MemoryStream::Init()
	{
		m_buffer = new uint8_t[m_capacity];
		m_length = 0;
		m_position = 0;
		m_origin = 0;
		m_isOpen = true;
		m_expandable = true;
		m_writable = true;
	}

	MemoryStream::~MemoryStream()
	{
	}

	long MemoryStream::GetPosition()
	{
		return (long)(m_position - m_origin);
	}

	long MemoryStream::GetLength()
	{
		return (long)(m_length - m_origin);
	}

	bool MemoryStream::EnsureCapacity(int value)
	{
		if (value <= m_capacity)
			return false;

		int num = value;
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
		delete[] m_buffer;

		m_capacity = num;
		m_buffer = newbuffer;
		return true;
	}

	uint8_t* MemoryStream::GetBuffer()
	{
		return m_buffer;
	}

	int MemoryStream::ReadByte()
	{
		if (m_position >= m_length)
		{
			return -1;
		}
		return m_buffer[m_position++];
	}

	void MemoryStream::WriteByte(uint8_t value)
	{
		if (m_position >= m_length)
		{
			int num = m_position + 1;
			bool flag = m_position > m_length;
			if ((num >= m_capacity) && EnsureCapacity(num))
			{
				flag = false;
			}
			if (flag)
			{
				//Esto no es necesario.
				memcpy(m_buffer + m_length, 0x00, m_position - m_length);
			}
			m_length = num;
		}
		m_buffer[m_position++] = value;
	}

	int MemoryStream::Read(uint8_t* buffer, int offset, int count)
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
				buffer[offset + num2] = m_buffer[m_position + num2];
			}
		}
		else
		{
			memcpy(buffer + m_position, m_buffer + offset, byteCount);
		}
		m_position += byteCount;
		return byteCount;
	}

	void MemoryStream::Write(uint8_t* buffer, int offset, int count)
	{
		int num = m_position + count;
		if (num < 0)
		{
			throw std::exception("stream too long");
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
				memcpy(buffer + m_length, 0x00, num - m_length);
			}
			m_length = num;
		}
		if ((count <= 8) && (buffer != m_buffer))
		{
			int num2 = count;
			while (--num2 >= 0)
			{
				m_buffer[m_position + num2] = buffer[offset + num2];
			}
		}
		else
		{
			memcpy(m_buffer + m_position, buffer + offset, count);
		}
		m_position = num;
	}

	long MemoryStream::Seek(long offset, SeekOrigin origin)
	{
		return 0;
	}
}