#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Bruno
{
	class Stream
	{
	public:
		virtual ~Stream() = default;
		virtual void Close() { }

		virtual long GetLength() = 0;
		virtual uint64_t GetPosition() = 0;
		virtual bool IsStreamValid() const = 0;

		virtual bool Read(uint8_t* destination, size_t count) = 0;

		void ReadBytes(std::vector<uint8_t>& bytes, size_t size = 0);

		template<typename T>
		void ReadRaw(T& object)
		{
			Read((uint8_t*)&object, sizeof(T));
		}
		void ReadString(std::string& string);

		virtual void Write(const uint8_t* source, size_t count) = 0;

		void WriteBytes(const std::vector<uint8_t>& bytes, bool writeSize = true);

		template<typename T>
		void WriteRaw(const T& object)
		{
			Write((uint8_t*)&object, sizeof(T));
		}
		void WriteString(const std::string& string);
	};
}