#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace BrunoEngine
{
	class BRUNO_API_EXPORT MaterialReader : public ContentTypeReader
	{
	public:
		MaterialReader();
		~MaterialReader();

		const char* GetReaderName() { return "MaterialReader"; }
		void* Read(ContentReader* input);
	};
	
}