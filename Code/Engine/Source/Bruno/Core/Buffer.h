#pragma once

#include <cstdint>

namespace Bruno
{
	struct Buffer
	{
		size_t Size;
		uint8_t* Data;

		Buffer(size_t size) :
			Data(nullptr),
			Size(0)
		{
			EnsureCapacity(size);
		}

		bool EnsureCapacity(size_t size)
		{
			if (size <= Size)
				return false;

			size_t newCapacity = size;
			if (newCapacity < 4096)
			{
				newCapacity = 4096;
			}
			if (newCapacity < (Size * 2))
			{
				newCapacity = Size * 2;
			}
			uint8_t* newbuffer = new uint8_t[newCapacity];

			if (Size > 0)
			{
				memcpy(newbuffer, Data, Size);
			}

			if (Data)
				delete[] Data;

			Size = newCapacity;
			Data = newbuffer;
			return true;
		}
	};
}