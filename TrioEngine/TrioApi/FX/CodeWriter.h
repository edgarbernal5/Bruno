#pragma once

#include "TrioApiDecl.h"

#include <string>

#if defined(__GNUC__)
#define M4_PRINTF_ATTR(string_index, first_to_check) __attribute__((format(printf, string_index, first_to_check)))
#else
#define M4_PRINTF_ATTR(string_index, first_to_check)
#endif

namespace TrioFX
{
	class TRIOAPI_DECL CodeWriter
	{

	public:
		CodeWriter(bool writeFileNames = true);

		void BeginLine(int indent, const char* fileName = nullptr, int lineNumber = -1);
		M4_PRINTF_ATTR(2, 3) void Write(const char* format, ...);
		void EndLine(const char* text = nullptr);

		M4_PRINTF_ATTR(3, 4) void WriteLine(int indent, const char* format, ...);
		M4_PRINTF_ATTR(5, 6) void WriteLineTagged(int indent, const char* fileName, int lineNumber, const char* format, ...);

		const char* GetResult() const;
		
		void Reset();
	private:

		std::string     m_buffer;
		int             m_currentLine;
		const char*     m_currentFileName;
		int             m_spacesPerIndent;
		bool            m_writeLines;
		bool            m_writeFileNames;

	};
}