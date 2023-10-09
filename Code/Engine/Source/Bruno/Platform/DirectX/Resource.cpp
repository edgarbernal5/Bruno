#include "brpch.h"
#include "Resource.h"

#include "D3D12MemAlloc.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Resource);

	Resource::~Resource()
	{
		SafeRelease(mResource);
	}
}