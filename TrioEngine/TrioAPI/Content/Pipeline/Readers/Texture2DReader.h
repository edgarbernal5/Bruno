#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace TrioEngine
{
	class TRIO_API_EXPORT Texture2DReader : public ContentTypeReader
	{
	public:
		Texture2DReader();
		~Texture2DReader();

		const char* GetReaderName() { return "Texture2DReader"; }
		void* Read(ContentReader* input);
	};
	
}