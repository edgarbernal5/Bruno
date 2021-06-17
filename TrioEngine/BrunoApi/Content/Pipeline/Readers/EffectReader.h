#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace BrunoEngine
{
	class BRUNO_API_EXPORT EffectReader : public ContentTypeReader
	{
	public:
		EffectReader();
		~EffectReader();

		const char* GetReaderName() { return "EffectReader"; }
		void* Read(ContentReader* input);
	};
	
}