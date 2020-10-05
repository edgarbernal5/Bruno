#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace TrioEngine
{
	class TRIO_API_EXPORT VertexBufferReader : public ContentTypeReader
	{
	public:
		VertexBufferReader();
		~VertexBufferReader();

		const char* GetReaderName() { return "VertexBufferReader"; }
		void* Read(ContentReader* input);
	};
	
}