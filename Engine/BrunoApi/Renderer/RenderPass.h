#pragma once

#include "TrioApiRequisites.h"
#include <vector>

namespace BrunoEngine
{
	struct RenderPassSetup {

	};

	struct RenderPass {
		std::vector<RenderPassSetup> m_setups;
	};
}