#pragma once

#include "TrioApiRequisites.h"

#include "Stream.h"

namespace BrunoIO
{
	class BRUNO_API_EXPORT MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(int capacity);
		~MemoryStream();

		int Read(uint8_t* buffer, int offset, int count);
		void Write(uint8_t* buffer, int offset, int count);

		uint8_t* GetBuffer();

		int ReadByte();
		void WriteByte(uint8_t value);
		long GetLength();
		long GetPosition();
		bool CanRead()
		{
			return m_isOpen;
		}

		long Seek(long offset, SeekOrigin origin);
		bool CanSeek()
		{
			return m_isOpen;
		}
	private:
		uint8_t* m_buffer;
		uint32_t m_capacity;
		int m_length;
		int m_position;
		int m_origin;
		bool EnsureCapacity(int value);

		bool m_isOpen;

		bool m_expandable;
		bool m_writable;

		void Init();
	};
}