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
		None = (1 << 0),
		Read = (1 << 1),
		Write = (1 << 2)
	};
	BR_DEFINITION_ENUM_FLAG(FileAccess);

	class FileStream : public Stream
	{
	public:
		FileStream(const std::wstring& filename, FileAccess fileAccess);
		~FileStream() = default;

		virtual void Close() override;

		virtual int Read(uint8_t* buffer, int offset, int count) override;
		virtual void Write(uint8_t* buffer, int offset, int count) override;

	private:

		std::fstream m_stream;
	};	
}
