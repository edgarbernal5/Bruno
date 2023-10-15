#include "brpch.h"
#include "Context.h"

#include "GraphicsDevice.h"
#include "Resource.h"

namespace Bruno
{
	Context::Context(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType) :
		mDevice(device),
		mContextType(commandType)
	{
		for (uint32_t frameIndex = 0; frameIndex < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; frameIndex++)
		{
			ThrowIfFailed(device.GetD3DDevice()->CreateCommandAllocator(commandType, IID_PPV_ARGS(&mCommandAllocators[frameIndex])));
		}
		ThrowIfFailed(mDevice.GetD3DDevice()->CreateCommandList1(0, commandType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&mCommandList)));
	}
	
	Context::~Context()
	{
		SafeRelease(mCommandList);

		for (uint32_t frameIndex = 0; frameIndex < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; frameIndex++)
		{
			SafeRelease(mCommandAllocators[frameIndex]);
		}
	}

	void Context::AddBarrier(Resource& resource, D3D12_RESOURCE_STATES newState)
	{
		if (mNumQueuedBarriers >= Graphics::Core::MAX_QUEUED_BARRIERS)
		{
			FlushBarriers();
		}

		D3D12_RESOURCE_STATES oldState = resource.mState;
		if (mContextType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			constexpr D3D12_RESOURCE_STATES VALID_COMPUTE_CONTEXT_STATES = (D3D12_RESOURCE_STATE_UNORDERED_ACCESS | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
				D3D12_RESOURCE_STATE_COPY_DEST | D3D12_RESOURCE_STATE_COPY_SOURCE);

			BR_ASSERT((oldState & VALID_COMPUTE_CONTEXT_STATES) == oldState);
			BR_ASSERT((newState & VALID_COMPUTE_CONTEXT_STATES) == newState);
		}

		if (oldState != newState)
		{
			D3D12_RESOURCE_BARRIER& barrierDesc = mResourceBarriers[mNumQueuedBarriers];
			++mNumQueuedBarriers;

			barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrierDesc.Transition.pResource = resource.mResource;
			barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrierDesc.Transition.StateBefore = oldState;
			barrierDesc.Transition.StateAfter = newState;
			barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			resource.mState = newState;
		}
		else if (newState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			D3D12_RESOURCE_BARRIER& barrierDesc = mResourceBarriers[mNumQueuedBarriers];
			++mNumQueuedBarriers;

			barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrierDesc.UAV.pResource = resource.mResource;
		}
	}

	void Context::FlushBarriers()
	{
		if (mNumQueuedBarriers > 0)
		{
			mCommandList->ResourceBarrier(mNumQueuedBarriers, mResourceBarriers.data());
			mNumQueuedBarriers = 0;
		}
	}

	void Context::CopyResource(const Resource& destination, const Resource& source)
	{
		mCommandList->CopyResource(destination.mResource, source.mResource);
	}

	void Context::CopyBufferRegion(Resource& destination, uint64_t destOffset, Resource& source, uint64_t sourceOffset, uint64_t numBytes)
	{
		mCommandList->CopyBufferRegion(destination.mResource, destOffset, source.mResource, sourceOffset, numBytes);
	}

	void Context::CopyTextureRegion(Resource& destination, Resource& source, size_t sourceOffset, SubResourceLayouts& subResourceLayouts, uint32_t numSubResources)
	{
		for (uint32_t subResourceIndex = 0; subResourceIndex < numSubResources; subResourceIndex++)
		{
			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = destination.mResource;
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destinationLocation.SubresourceIndex = subResourceIndex;

			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = source.mResource;
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint = subResourceLayouts[subResourceIndex];
			sourceLocation.PlacedFootprint.Offset += sourceOffset;

			mCommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
		}
	}

	void Context::Reset()
	{
		uint32_t frameId = mDevice.GetFrameId();

		mCommandAllocators[frameId]->Reset();
		mCommandList->Reset(mCommandAllocators[frameId], nullptr);

		if (mContextType != D3D12_COMMAND_LIST_TYPE_COPY)
		{
			BindDescriptorHeaps(mDevice.GetFrameId());
		}
	}

	void Context::BindDescriptorHeaps(uint32_t frameIndex)
	{
		mCurrentSRVHeap = &mDevice.GetSrvDescriptionHeap(frameIndex);
		mCurrentSRVHeap->Reset();

		ID3D12DescriptorHeap* heapsToBind[2];
		heapsToBind[0] = mDevice.GetSrvDescriptionHeap(frameIndex).GetHeap();
		//heapsToBind[1] = mDevice.GetSamplerHeap().GetHeap();

		mCommandList->SetDescriptorHeaps(1, heapsToBind);
	}
}