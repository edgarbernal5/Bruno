#pragma once

#include <cstdint>

namespace Bruno
{
	class Stream
	{
	public:
		virtual ~Stream() = default;
		virtual void Close() { }

		virtual void Write(uint8_t* buffer, int count) = 0;
		virtual int Read(uint8_t* buffer, int count) = 0;

		virtual int ReadByte()
		{
			uint8_t buffer[1];
			if (Read(buffer, 1) == 0)
			{
				return -1;
			}
			return buffer[0];
		}

		virtual void WriteByte(uint8_t value)
		{
			uint8_t buffer[1] = { value };

			Write(buffer, 1);
		}
	};
}