#pragma once

#include <zlib.h>
#include "Stream.h"

namespace Bruno
{
	class MemoryStream;

	class Compressor
	{
	public:
		Compressor(Stream& stream);
		~Compressor();

		void Compress(uint8_t* inputBytes, unsigned long sizeInBytes);
		void FlushOutput();

	private:
		Stream& m_stream;
		uint8_t* m_buffer;
		uint32_t m_capacity;
		uint32_t m_length;
	};

	class Decompressor
	{
	public:
		Decompressor(MemoryStream* stream);

		void Decompress(Stream* source, int compressedLength, int decompressedLength);

	private:
		MemoryStream* m_stream;
	};
}

