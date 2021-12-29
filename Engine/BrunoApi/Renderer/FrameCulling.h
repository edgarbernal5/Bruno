#pragma once

#include "TrioApiRequisites.h"
#include <vector>

namespace BrunoEngine
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