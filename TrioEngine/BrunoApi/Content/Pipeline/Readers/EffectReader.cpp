#include "stdafx.h"
#include "EffectReader.h"

#include "Content/GraphicsContentHelper.h"
#include "Content/ContentReader.h"

#include "Graphics/Effect.h"

namespace BrunoEngine
{
	EffectReader::EffectReader()
	{
	}


	EffectReader::~EffectReader()
	{
	}

	void* EffectReader::Read(ContentReader* input)
	{
		Effect* effect = new Effect(GraphicsContentHelper::GraphicsDeviceFromContentReader(input));
		uint32_t size = input->ReadUInt32();

		return nullptr;
	}
}