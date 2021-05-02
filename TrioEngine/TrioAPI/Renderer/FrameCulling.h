#pragma once

#include "TrioApiRequisites.h"
#include <vector>

namespace TrioEngine
{
	struct FrameCulling
	{
		std::vector<uint32_t> m_culledObjects;

		void Clear()
		{
			m_culledObjects.clear();
		}
	};
}