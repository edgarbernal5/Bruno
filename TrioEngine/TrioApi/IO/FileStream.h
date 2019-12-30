#pragma once

#include "TrioApiRequisites.h"

#include "Stream.h"

#include <iostream>
#include <fstream>

namespace TrioIO
{
	enum class FileAccess
	{
		None = 0,
		Read = 1,
		Write = 2
	};
	DEFINE_ENUM_FLAG_OPERATORS(FileAccess)

	class TRIO_API_EXPORT FileStream : public Stream
	{
	public:
		FileStream(std::string filename, FileAccess access);
		~FileStream();

		int Read(uint8_t* buffer, int offset, int count) override;
		void Write(uint8_t* buffer, int offset, int count) override;

		long Seek(long offset, SeekOrigin origin);

		void Close();
		long GetLength();
		bool CanSeek();
		long GetPosition();

		int ReadByte() override;

		bool CanRead();
	private:
		std::string m_FileName;
		FileAccess m_Access;

		long m_nativeLength;
		bool m_canSeek;
		bool m_canRead;

		long m_pos;
		int m_readLen;
		int m_readPos;
		int m_writePos;
		long m_appendStart;

		int m_bufferSize;
		uint8_t* m_buffer;

		std::fstream m_Stream;
		void WriteCore(uint8_t* buffer, int offset, int count);
		void FlushWrite(bool calledFromFinalizer);
		int ReadCore(uint8_t* buffer, int offset, int count);

	};
}