#include "brpch.h"
#include "Compressor.h"

namespace Bruno
{
	Compressor::Compressor(Stream& stream) :
		m_stream(stream)
	{
	}

	void Compressor::Compress(uint8_t* inputBytes, unsigned long sizeInBytes)
	{
		unsigned long compressedSize = compressBound(sizeInBytes);
		unsigned char* compressedBytes = new unsigned char[compressedSize];

		int result = compress(compressedBytes, &compressedSize, inputBytes, sizeInBytes);

		if (result == Z_OK)
		{
			BR_CORE_TRACE << "compression successfull." << std::endl;
			m_stream.Write(compressedBytes, compressedSize);
		}
		else
		{
			// Handle compression error
			BR_CORE_TRACE << " compression error." << std::endl;
		}
		delete[] compressedBytes;
	}


	Decompressor::Decompressor(Stream& stream) :
		m_stream(stream)
	{
	}
}