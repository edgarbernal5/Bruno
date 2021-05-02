#pragma once

#include "TrioApiRequisites.h"
#include <vector>

namespace TrioEngine
{
	struct RenderPassSetup {

	};

	struct RenderPass {
		std::vector<RenderPassSetup> m_setups;
	};
}