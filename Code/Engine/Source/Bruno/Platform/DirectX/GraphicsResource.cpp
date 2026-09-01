#include "brpch.h"
#include "GraphicsResource.h"

#include "D3DHelpers.h"
#include "D3D12MemAlloc.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(GraphicsResource);

	GraphicsResource::~GraphicsResource()
	{
		SafeRelease(m_resource);
		SafeRelease(m_allocation);
	}
}