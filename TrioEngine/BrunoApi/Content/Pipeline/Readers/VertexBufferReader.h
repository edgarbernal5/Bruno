#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace BrunoEngine
{
	class BRUNO_API_EXPORT VertexBufferReader : public ContentTypeReader
	{
	public:
		VertexBufferReader();
		~VertexBufferReader();

		const char* GetReaderName() { return "VertexBufferReader"; }
		void* Read(ContentReader* input);
	};
	
}