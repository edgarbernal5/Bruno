#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <vector>
#include <stdint.h>

namespace TrioIO
{
	class Stream;

	class TRIO_API_EXPORT BinaryReader
	{
	public:
		BinaryReader(Stream* stream);
		virtual ~BinaryReader()
		{
			if (m_buffer != nullptr)
				delete[] m_buffer;

			m_buffer = nullptr;
		}

		uint8_t ReadByte();
		uint16_t ReadUInt16();
		uint32_t ReadUInt32();
		uint64_t ReadUInt64();

		int8_t ReadSByte();
		int16_t ReadInt16();
		int32_t ReadInt32();
		int64_t ReadInt64();

		float ReadSingle();
		double ReadDouble();

		bool ReadBoolean();

		std::string ReadString();
		char ReadChar();

		virtual std::vector<uint8_t> ReadBytes(int count);

		int Read();
		virtual int Read(uint8_t* buffer, int index, int count);
	protected:
		Stream* m_Stream;
		void FillBuffer(int numBytes);
	private:
		int InternalReadOneChar();
		uint8_t* m_buffer;
	};
}