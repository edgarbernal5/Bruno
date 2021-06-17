#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace TrioEngine
{
	class BRUNO_API_EXPORT IndexCollectionReader : public ContentTypeReader
	{
	public:
		IndexCollectionReader();
		~IndexCollectionReader();

		const char* GetReaderName() { return "IndexCollectionReader"; }
		void* Read(ContentReader* input);
	};
	
}