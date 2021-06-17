#include "stdafx.h"
#include "HLSLPreprocessor.h"

namespace TrioFX
{

	HLSLPreprocessor::HLSLPreprocessor(const char* buffer, size_t length) : m_buffer(buffer)
	{
		m_bufferEnd = m_buffer + length;
		Preprocess();
	}


	HLSLPreprocessor::~HLSLPreprocessor()
	{
	}

	void HLSLPreprocessor::Preprocess()
	{
		if (m_bufferEnd - m_buffer > 8 && strncmp(m_buffer, "#include", 8) == 0 && isspace(m_buffer[8]))
		{
			m_buffer += 8;

			while (m_buffer < m_bufferEnd && isspace(m_buffer[0]))
			{
				if (m_buffer[0] == '\n')
				{
					//Error("Syntax error: expected line number after #include");
					break;
				}
				++m_buffer;
			}

			char* iEnd = nullptr;
		}
	}

}