#include "brpch.h"
#include "MemoryStream.h"

#include <string.h>
#include <exception>

namespace Bruno
{
	MemoryStream::MemoryStream() :
		m_buffer(0),
		m_position(0)
	{
		m_length = 0;
		EnsureCapacity(4096);
	}

	MemoryStream::MemoryStream(Buffer& buffer) :
		m_buffer(buffer),
		m_position(0)
	{
		m_length = 0;
		EnsureCapacity(4096);
	}

	MemoryStream::MemoryStream(uint32_t capacity) :
		m_buffer(capacity),
		m_position(0)
	{
		m_length = 0;
		EnsureCapacity(capacity);
	}

	MemoryStream::~MemoryStream()
	{
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

	void MemoryStream::SetPosition(uint64_t position)
	{
		m_position = position;
	}

	bool MemoryStream::Read(uint8_t* destination, size_t count)
	{
		if (m_position + count > m_buffer.Size)
			return false;

		memcpy(destination, m_buffer.Data + m_position, count);
		m_position += count;
		return true;
	}

	void MemoryStream::Write(const uint8_t* source, size_t count)
	{
		if (m_position + count > m_buffer.Size)
		{
			EnsureCapacity(m_position + count);
		}

		memcpy(m_buffer.Data + m_position, source, count);
		m_length += static_cast<long>(count);
		m_position += static_cast<uint64_t>(count);
	}

	bool MemoryStream::EnsureCapacity(uint64_t capacity)
	{
		return m_buffer.EnsureCapacity(capacity);
	}
}