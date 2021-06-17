#pragma once

#include "TrioApiRequisites.h"
#include <stdint.h>

namespace TrioEngine
{
	typedef uint64_t Entity;

	static const Entity INVALID_ENTITY = 0;

	static Entity g_currentEntityIndex = 0;

	inline Entity CreateEntity()
	{
		return ++g_currentEntityIndex;
	}

	inline void ResetEntities() {
		g_currentEntityIndex = 0;
	}
}