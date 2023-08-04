#include "brpch.h"
#include "Stream.h"

namespace Bruno
{
	void Stream::ReadBytes(std::vector<uint8_t>& bytes, size_t size)
	{
		if (size == 0)
			ReadRaw<size_t>(size);

		bytes.resize(size);
		Read(bytes.data(), size);
	}

	void Stream::ReadString(std::string& string)
	{
		size_t size;
		Read((uint8_t*)&size, sizeof(size_t));

		string.resize(size);
		Read((uint8_t*)string.data(), sizeof(char) * size);
	}

	void Stream::WriteBytes(const std::vector<uint8_t>& bytes, bool writeSize)
	{
		if (writeSize)
		{
			size_t size = bytes.size();
			Write((uint8_t*)&size, sizeof(size_t));
		}

		Write(bytes.data(), sizeof(uint8_t) * bytes.size());
	}
	
	void Stream::WriteString(const std::string& string)
	{
		size_t size = string.size();
		Write((uint8_t*)&size, sizeof(size_t));
		Write((uint8_t*)string.data(), sizeof(char) * string.size());
	}
}
