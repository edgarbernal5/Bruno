#pragma once


#include "CommonHeader.h"
#include <string>

using namespace std;

namespace CuadoFX
{

	class TRIOFXAPI_DLL HLSLPreprocessor
	{
	public:
		HLSLPreprocessor(const char* buffer, size_t length);
		~HLSLPreprocessor();


	private:
		void Preprocess();

		const char* m_buffer;
		const char* m_bufferEnd;
		string m_result;
	};

}