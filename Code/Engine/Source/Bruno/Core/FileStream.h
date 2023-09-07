#pragma once

#include "Stream.h"
#include "Base.h"
#include <string>

#include <fstream>

namespace Bruno
{
	enum class FileAccess : uint8_t
	{
		None = 0,
		Read = 1,
		Write = 2
	};
	BR_DEFINITION_FLAG_FROM_ENUM(FileAccess);

	class FileStream : public Stream
	{
	public:
		FileStream(const std::wstring& filename, FileAccess fileAccess);
		~FileStream();

		void Close() override;

		virtual long GetLength() override;
		virtual uint64_t GetPosition() override;
		virtual bool IsStreamValid() const override;
		void SetPosition(uint64_t position) override;

		bool Read(uint8_t* destination, size_t count) override;

		void Write(const uint8_t* buffer, size_t count) override;

	private:
		long m_fileLength;
		std::wstring m_filename;
		FileAccess m_fileAccess;
		std::fstream m_stream;
	};	
}
