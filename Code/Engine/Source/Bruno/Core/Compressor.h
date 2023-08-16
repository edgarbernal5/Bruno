#pragma once

#include <zlib.h>
#include "Stream.h"

namespace Bruno
{
	class Compressor
	{
	public:
		Compressor(Stream& stream);

		void Compress(uint8_t* inputBytes, unsigned long sizeInBytes);

	private:
		Stream& m_stream;
	};

	class Decompressor
	{
	public:
		Decompressor(Stream& stream);

		//void Decompress(uint8_t* inputBytes, unsigned long sizeInBytes);

	private:
		Stream& m_stream;
	};
}

