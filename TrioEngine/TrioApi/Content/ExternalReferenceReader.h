#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include "ContentTypeReader.h"

namespace TrioEngine
{
	class ContentReader;

	class TRIO_API_EXPORT ExternalReferenceReader : public ContentTypeReader
	{
	public:
		ExternalReferenceReader();
		~ExternalReferenceReader();

		const char* GetReaderName() { return "ExternalReferenceReader"; };
		void* Read(ContentReader* input);
	};
}