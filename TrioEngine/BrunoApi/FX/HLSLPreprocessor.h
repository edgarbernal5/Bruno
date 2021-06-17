#pragma once


#include "TrioApiRequisites.h"

#include <string>

namespace TrioFX
{

	class BRUNO_API_EXPORT HLSLPreprocessor
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