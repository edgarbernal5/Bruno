#pragma once

#include "Stream.h"
#include "Buffer.h"

namespace Bruno
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(Buffer& buffer);
		MemoryStream(uint32_t capacity);
		~MemoryStream();

		Buffer& GetBuffer() { return m_buffer; }

		long GetLength() override;
		uint64_t GetPosition() override;
		bool IsStreamValid() const override;
		void SetPosition(uint64_t position) override;

		virtual bool Read(uint8_t* destination, size_t count) override;

		virtual void Write(const uint8_t* source, size_t count) override;

	private:
		Buffer m_buffer;
		long m_length;
		uint64_t m_position;

		bool EnsureCapacity(uint64_t value);
	};
}


