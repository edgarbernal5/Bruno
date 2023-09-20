#pragma once

#include "Context.h"

namespace Bruno
{
	class GraphicsContext : public Context
	{
	public:
		GraphicsContext(GraphicsDevice& device);
		~GraphicsContext();
	};
}

