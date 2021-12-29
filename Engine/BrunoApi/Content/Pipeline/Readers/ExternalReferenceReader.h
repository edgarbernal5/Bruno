#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace BrunoEngine
{
	class BRUNO_API_EXPORT ExternalReferenceReader : public ContentTypeReader
	{
	public:
		ExternalReferenceReader();
		~ExternalReferenceReader();

		const char* GetReaderName() { return "ExternalReferenceReader"; }
		void* Read(ContentReader* input);

	};
	
}