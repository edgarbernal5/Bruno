#include "brpch.h"
#include "GraphicsContext.h"

namespace Bruno
{
	GraphicsContext::GraphicsContext(GraphicsDevice& device) :
		Context(device, D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
	}
}