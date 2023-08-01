#pragma once

#include "Stream.h"

namespace Bruno
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(uint32_t capacity);
		~MemoryStream() = default;

		int ReadByte() override;
		virtual int Read(uint8_t* buffer, int count) override;

		void WriteByte(uint8_t byte) override;
		virtual void Write(uint8_t* buffer, int count) override;

	private:
		uint8_t* m_buffer;
		uint32_t m_capacity;
		int m_length;
		int m_position;

		bool EnsureCapacity(uint32_t value);
	};
}


