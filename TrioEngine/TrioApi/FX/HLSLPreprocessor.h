#pragma once


#include "TrioApiDecl.h"

#include <string>

namespace TrioFX
{

	class TRIOAPI_DECL HLSLPreprocessor
	{
	public:
		HLSLPreprocessor(const char* buffer, size_t length);
		~HLSLPreprocessor();


	private:
		void Preprocess();

		const char* m_buffer;
		const char* m_bufferEnd;
		std::string m_result;
	};

}