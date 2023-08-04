#pragma once

#include "Stream.h"
#include <string>

#include <fstream>

namespace Bruno
{
#define BR_DEFINITION_ENUM_FLAG(flag) \
	inline constexpr enum flag operator |(const enum flag selfValue, const enum flag inValue) \
	{ \
		return (enum flag)(uint32_t(selfValue) | uint32_t(inValue)); \
	} \
	inline constexpr enum flag operator &(const enum flag selfValue, const enum flag inValue) \
	{ \
		return (enum flag)(uint32_t(selfValue) & uint32_t(inValue)); \
	}

	enum class FileAccess : uint32_t
	{
		None = 0,
		Read = 1,
		Write = 2
	};
	BR_DEFINITION_ENUM_FLAG(FileAccess);

	class FileStream : public Stream
	{
	public:
		FileStream(const std::wstring& filename, FileAccess fileAccess);
		~FileStream();

		void Close() override;

		virtual long GetLength() override;
		virtual uint64_t GetPosition() override;
		virtual bool IsStreamValid() const override;

		bool Read(uint8_t* destination, size_t count) override;

		void Write(const uint8_t* buffer, size_t count) override;

	private:
		long m_fileLength;
		std::fstream m_stream;
	};	
}
