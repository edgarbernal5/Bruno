#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <stdint.h>

namespace TrioIO
{
	class Stream;

	class TRIO_API_EXPORT BinaryWriter
	{
	public:
		BinaryWriter(Stream* _stream);
		virtual ~BinaryWriter() { }

		void WriteInt32(int value);
		void WriteChar(char value);
		void WriteBool(bool value);
		void WriteSingle(float value);
		void WriteDouble(double value);
		void WriteByte(uint8_t value);
		void WriteBytes(uint8_t *value, size_t size);
		void WriteShort(uint16_t value);
		void WriteUInt32(uint32_t value);
		void WriteString(std::string value);

		void Close();
	private:

	protected:
		Stream* m_Stream;

		uint8_t m_Buffer[0x10];

		void WriteBuffer(uint8_t* buffer, int offset, int length);
	};
}