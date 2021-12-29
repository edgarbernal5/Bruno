#include "stdafx.h"
#include "CompiledEffectContent.h"

namespace BrunoEngine
{
	CompiledEffectContent::CompiledEffectContent()
	{
	}

	CompiledEffectContent::CompiledEffectContent(std::vector<uint8_t>& bytes) :
		m_bytes(bytes)
	{

	}

	CompiledEffectContent::~CompiledEffectContent()
	{
	}
}