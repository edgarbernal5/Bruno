#include "brpch.h"
#include "Compressor.h"

#include "MemoryStream.h"

namespace Bruno
{
	Compressor::Compressor(Stream& stream) :
		m_stream(stream),
		m_buffer(nullptr),
		m_capacity(0),
		m_length(0)
	{
	}

	Compressor::~Compressor()
	{
		if (m_buffer) {
			delete[] m_buffer;
			m_buffer = nullptr;
			m_capacity = 0;
			m_length = 0;
		}
	}

	void Compressor::Compress(uint8_t* inputBytes, unsigned long sizeInBytes)
	{
		if (sizeInBytes > m_capacity)
		{
			unsigned long newCapacity = sizeInBytes + m_length;
			if (newCapacity < 4096)
			{
				newCapacity = 4096;
			}
			if (newCapacity < (m_capacity * 2))
			{
				newCapacity = m_capacity * 2;
			}

			uint8_t* newBuffer = new uint8_t[newCapacity];

			if (m_length > 0)
				memcpy(newBuffer, m_buffer, m_length);

			if (m_buffer)
				delete[] m_buffer;

			m_buffer = newBuffer;
			m_capacity = newCapacity;
		}

		memcpy(m_buffer + m_length, inputBytes, sizeInBytes);
		m_length += sizeInBytes;
	}

	void Compressor::FlushOutput()
	{
		unsigned long compressedSize = compressBound(m_length);
		unsigned char* compressedBytes = new unsigned char[compressedSize];

		int result = compress(compressedBytes, &compressedSize, m_buffer, m_length);

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

	Decompressor::Decompressor(MemoryStream* stream) :
		m_stream(stream)
	{
		
	}

	void Decompressor::Decompress(Stream* source, int compressedLength, int decompressedLength)
	{
		std::vector<uint8_t> buffer;
		source->ReadBytes(buffer, compressedLength);

		unsigned long finalL = decompressedLength;
		int result = uncompress(m_stream->GetBuffer(), &finalL, buffer.data(), compressedLength);
		if (result == Z_OK)
		{
		}
	}
}