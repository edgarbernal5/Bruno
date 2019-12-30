#pragma once

#include <string>

namespace TrioIO
{
	enum class SeekOrigin
	{
		Begin,
		Current,
		End
	};

	class TRIO_API_EXPORT Stream
	{
	public:
		Stream() {}
		virtual ~Stream() {}

		virtual void Close() { }
		//virtual void Flush() = 0;

		virtual int Read(uint8_t* buffer, int offset, int count) = 0;

		virtual long Seek(long offset, SeekOrigin origin) = 0;
		//virtual void SetLength(long value) = 0;

		virtual void Write(uint8_t* buffer, int offset, int count) = 0;

		virtual bool CanRead() = 0;
		virtual bool CanSeek() = 0;
		//virtual bool CanWrite() = 0;
		virtual long GetLength() = 0;
		virtual long GetPosition() = 0;

		virtual int ReadByte()
		{
			uint8_t buffer[1];
			if (Read(buffer, 0, 1) == 0)
			{
				return -1;
			}
			return buffer[0];
		}

		virtual void WriteByte(uint8_t value)
		{
			uint8_t buffer[1] = { value };

			Write(buffer, 0, 1);
		}
	protected:
		bool m_validStream;
	};
}