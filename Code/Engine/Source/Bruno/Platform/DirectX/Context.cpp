#include "brpch.h"
#include "Context.h"

#include "D3DHelpers.h"
#include "GraphicsDevice.h"
#include "Resource.h"

namespace Bruno
{
	Context::Context(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType) :
		m_device(device),
		m_contextType(commandType)
	{
		for (uint32_t frameIndex = 0; frameIndex < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; frameIndex++)
		{
			ThrowIfFailed(device.GetD3DDevice()->CreateCommandAllocator(commandType, IID_PPV_ARGS(&m_commandAllocators[frameIndex])));
		}
		ThrowIfFailed(m_device.GetD3DDevice()->CreateCommandList1(0, commandType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_commandList)));
	}
	
	Context::~Context()
	{
		SafeRelease(m_commandList);

		for (uint32_t frameIndex = 0; frameIndex < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; frameIndex++)
		{
			SafeRelease(m_commandAllocators[frameIndex]);
		}
	}

	void Context::AddBarrier(Resource& resource, D3D12_RESOURCE_STATES newState)
	{
		if (m_totalQueuedBarriers >= Graphics::Core::MAX_QUEUED_BARRIERS)
		{
			FlushBarriers();
		}

		D3D12_RESOURCE_STATES oldState = resource.m_state;
		if (m_contextType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			constexpr D3D12_RESOURCE_STATES VALID_COMPUTE_CONTEXT_STATES = (D3D12_RESOURCE_STATE_UNORDERED_ACCESS | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
				D3D12_RESOURCE_STATE_COPY_DEST | D3D12_RESOURCE_STATE_COPY_SOURCE);

			BR_ASSERT((oldState & VALID_COMPUTE_CONTEXT_STATES) == oldState);
			BR_ASSERT((newState & VALID_COMPUTE_CONTEXT_STATES) == newState);
		}

		if (oldState != newState)
		{
			D3D12_RESOURCE_BARRIER& barrierDesc = m_resourceBarriers[m_totalQueuedBarriers];
			++m_totalQueuedBarriers;

			barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrierDesc.Transition.pResource = resource.m_resource;
			barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrierDesc.Transition.StateBefore = oldState;
			barrierDesc.Transition.StateAfter = newState;
			barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			resource.m_state = newState;
		}
		else if (newState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			D3D12_RESOURCE_BARRIER& barrierDesc = m_resourceBarriers[m_totalQueuedBarriers];
			++m_totalQueuedBarriers;

			barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrierDesc.UAV.pResource = resource.m_resource;
		}
	}

	void Context::FlushBarriers()
	{
		if (m_totalQueuedBarriers > 0)
		{
			m_commandList->ResourceBarrier(m_totalQueuedBarriers, m_resourceBarriers.data());
			m_totalQueuedBarriers = 0;
		}
	}

	void Context::CopyResource(const Resource& destination, const Resource& source)
	{
		m_commandList->CopyResource(destination.m_resource, source.m_resource);
	}

	void Context::CopyBufferRegion(Resource& destination, uint64_t destOffset, Resource& source, uint64_t sourceOffset, uint64_t numBytes)
	{
		m_commandList->CopyBufferRegion(destination.m_resource, destOffset, source.m_resource, sourceOffset, numBytes);
	}

	void Context::CopyTextureRegion(Resource& destination, Resource& source, size_t sourceOffset, SubResourceLayouts& subResourceLayouts, uint32_t numSubResources)
	{
		for (uint32_t subResourceIndex = 0; subResourceIndex < numSubResources; subResourceIndex++)
		{
			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = destination.m_resource;
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destinationLocation.SubresourceIndex = subResourceIndex;

			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = source.m_resource;
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint = subResourceLayouts[subResourceIndex];
			sourceLocation.PlacedFootprint.Offset += sourceOffset;

			m_commandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
		}
	}

	void Context::Reset()
	{
		uint32_t frameId = m_device.GetFrameId();

		m_commandAllocators[frameId]->Reset();
		m_commandList->Reset(m_commandAllocators[frameId], nullptr);

		if (m_contextType != D3D12_COMMAND_LIST_TYPE_COPY)
		{
			BindDescriptorHeaps(m_device.GetFrameId());
		}
	}

	void Context::BindDescriptorHeaps(uint32_t frameIndex)
	{
		m_currentSrvHeap = &m_device.GetSrvDescriptionHeap(frameIndex);
		m_currentSrvHeap->Reset();

		ID3D12DescriptorHeap* heapsToBind[2];
		heapsToBind[0] = m_device.GetSrvDescriptionHeap(frameIndex).GetHeap();
		//heapsToBind[1] = m_device.GetSamplerHeap().GetHeap();

		m_commandList->SetDescriptorHeaps(1, heapsToBind);
	}
}