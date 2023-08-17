#include "brpch.h"
#include "Compressor.h"

#include "MemoryStream.h"

namespace Bruno
{
	Compressor::Compressor(Stream& outputStream) :
		m_outputStream(outputStream),
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
			m_outputStream.Write(compressedBytes, compressedSize);
		}
		else
		{
			BR_CORE_TRACE << "Compression error. Result = " << result << std::endl;
		}
		delete[] compressedBytes;
	}

	Decompressor::Decompressor(MemoryStream* outputStream) :
		m_outputStream(outputStream)
	{
		
	}

	void Decompressor::Decompress(Stream* inputStream, int compressedLength, int decompressedLength)
	{
		std::vector<uint8_t> inputBuffer;
		inputStream->ReadBytes(inputBuffer, compressedLength);

		unsigned long decompressedSize = static_cast<unsigned long>(decompressedLength);
		std::vector<uint8_t> outputBuffer;
		outputBuffer.resize(decompressedSize);
		
		int result = uncompress(outputBuffer.data(), &decompressedSize, inputBuffer.data(), compressedLength);
		if (result == Z_OK)
		{
			m_outputStream->WriteBytes(outputBuffer, false);
			m_outputStream->SetPosition(0);
		}
	}
}