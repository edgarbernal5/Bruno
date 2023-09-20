#include "brpch.h"
#include "Context.h"

#include "GraphicsDevice.h"

namespace Bruno
{
	Context::Context(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType) :
		mDevice(device),
		mContextType(commandType)
	{
	}
	
	Context::~Context()
	{
		SafeRelease(mCommandList);
	}
}