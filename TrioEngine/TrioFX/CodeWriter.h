#pragma once

#include "CommonHeader.h"
#include <string>

using namespace std;

namespace CuadoFX
{
	class TRIOFXAPI_DLL CodeWriter
	{

	public:

		CodeWriter();

		void BeginLine(int indent, const char* fileName = nullptr, int lineNumber = -1);
		void Write(const char* format, ...);
		void EndLine(const char* text = nullptr);

		void WriteLine(int indent, const char* format, ...);
		void WriteLine(int indent, const char* fileName, int lineNumber, const char* format, ...);

		const char* GetResult() const;
		
		void Reset();
	private:

		string     m_buffer;
		int             m_currentLine;
		const char*     m_currentFileName;
		int             m_spacesPerIndent;
		bool            m_writeLines;
		bool            m_writeFileNames;

	};
}