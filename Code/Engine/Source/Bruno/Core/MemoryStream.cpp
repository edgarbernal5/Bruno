#include "brpch.h"
#include "MemoryStream.h"

#include <string.h>
#include <exception>

namespace Bruno
{
	MemoryStream::MemoryStream() :
		m_buffer(nullptr),
		m_capacity(0),
		m_position(0)
	{
		m_length = 0;
		EnsureCapacity(4096);
	}

	MemoryStream::MemoryStream(uint32_t capacity) :
		m_buffer(nullptr),
		m_capacity(capacity),
		m_position(0)
	{
		m_length = 0;
		EnsureCapacity(capacity);
	}

	MemoryStream::~MemoryStream()
	{
		if (m_buffer)
			delete[] m_buffer;

		m_buffer = nullptr;
		m_length = 0;
		m_position = 0;
	}

	long MemoryStream::GetLength()
	{
		return (long)(m_length);
	}

	uint64_t MemoryStream::GetPosition()
	{
		return m_position;
	}

	bool MemoryStream::IsStreamValid() const
	{
		return false;
	}

	bool MemoryStream::Read(uint8_t* destination, size_t count)
	{
		if (m_position + count > m_capacity)
			return false;

		memcpy(destination, (char*)m_buffer + m_position, count);
		m_position += count;
		return true;
	}

	void MemoryStream::Write(const uint8_t* buffer, size_t count)
	{
		if (m_position + count > m_capacity)
			return;

		memcpy(m_buffer + m_position, buffer, count);
		m_length += count;
		m_position += count;
	}

	bool MemoryStream::EnsureCapacity(uint64_t capacity)
	{
		if (capacity <= m_capacity)
			return false;

		uint64_t newCapacity = capacity;
		if (newCapacity < 4096)
		{
			newCapacity = 4096;
		}
		if (newCapacity < (m_capacity * 2))
		{
			newCapacity = m_capacity * 2;
		}
		uint8_t* newbuffer = new uint8_t[newCapacity];

		if (m_length > 0)
		{
			memcpy(newbuffer, m_buffer, m_length);
		}
		if (m_buffer)
			delete[] m_buffer;

		m_capacity = newCapacity;
		m_buffer = newbuffer;
		return true;
	}
}