#include "brpch.h"
#include "Stream.h"

namespace Bruno
{
	void Stream::ReadString(std::string& string)
	{
		size_t size;
		Read((uint8_t*)&size, sizeof(size_t));

		string.resize(size);
		Read((uint8_t*)string.data(), sizeof(char) * size);
	}
	
	void Stream::WriteString(const std::string& string)
	{
		size_t size = string.size();
		Write((uint8_t*)&size, sizeof(size_t));
		Write((uint8_t*)string.data(), sizeof(char) * string.size());
	}
}
