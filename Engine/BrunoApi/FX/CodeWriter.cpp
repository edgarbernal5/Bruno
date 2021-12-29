#include "stdafx.h"
#include "CodeWriter.h"

#include "AssertFX.h"
#include "../Utils/StringUtility.h"

#include <stdarg.h>

namespace BrunoFX
{
	static const int _maxLineLength = 2048;

	CodeWriter::CodeWriter(bool writeFileNames /*= true*/)
	{
		m_currentLine = 1;
		m_currentFileName = nullptr;
		m_spacesPerIndent = 4;
		m_writeLines = false;
		m_writeFileNames = writeFileNames;
	}

	void CodeWriter::BeginLine(int indent, const char* fileName, int lineNumber)
	{
		if (m_writeLines)
		{
			bool outputLine = false;
			bool outputFile = false;

			// Output a line number pragma if necessary.
			if (fileName != nullptr && m_currentFileName != fileName)
			{
				m_currentFileName = fileName;
				fileName = m_currentFileName;
				outputFile = true;
			}
			if (lineNumber != -1 && m_currentLine != lineNumber)
			{
				m_currentLine = lineNumber;
				outputLine = true;
			}
			if (outputLine || outputFile)
			{
				char buffer[256];
				TrioUtils::StringUtility::Printf(buffer, sizeof(buffer), "#line %d", lineNumber);
				m_buffer += buffer;
				if (outputFile && m_writeFileNames)
				{
					m_buffer += " \"";
					m_buffer += fileName;
					m_buffer += "\"\n";
				}
				else
				{
					m_buffer += "\n";
				}
			}

		}

		// Handle the indentation.
		for (int i = 0; i < indent * m_spacesPerIndent; ++i)
		{
			m_buffer += " ";
		}
	}

	void CodeWriter::EndLine(const char* text)
	{
		if (text != nullptr)
		{
			m_buffer += text;
		}
		m_buffer += "\n";
		++m_currentLine;
	}

	void CodeWriter::Write(const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		char buffer[_maxLineLength];
		TrioUtils::StringUtility::PrintfArgList(buffer, sizeof(buffer), format, args);

		m_buffer += buffer;

		va_end(args);
	}

	void CodeWriter::WriteLine(int indent, const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		char buffer[_maxLineLength];

		int result = TrioUtils::StringUtility::PrintfArgList(buffer, sizeof(buffer), format, args);
		ASSERT(result != -1);

		for (int i = 0; i < indent * m_spacesPerIndent; ++i)
		{
			m_buffer += " ";
		}
		m_buffer += buffer;

		EndLine();

		va_end(args);
	}

	void CodeWriter::WriteLineTagged(int indent, const char* fileName, int lineNumber, const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		BeginLine(indent, fileName, lineNumber);

		char buffer[_maxLineLength];
		int result = TrioUtils::StringUtility::PrintfArgList(buffer, sizeof(buffer), format, args);
		ASSERT(result != -1);

		m_buffer += buffer;

		EndLine();

		va_end(args);
	}

	const char* CodeWriter::GetResult() const
	{
		return m_buffer.c_str();
	}

	void CodeWriter::Reset()
	{
		m_buffer.clear();
	}
}