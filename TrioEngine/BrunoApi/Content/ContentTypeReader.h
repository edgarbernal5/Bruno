#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioEngine
{
	class ContentReader;

	class ContentTypeReader
	{
	public:
		ContentTypeReader() {}
		virtual ~ContentTypeReader() { }

		virtual const char* GetReaderName() = 0;
		virtual void* Read(ContentReader* input) = 0;

		//virtual void Initialize(class ContentTypeReaderManager*) { }
	};
}