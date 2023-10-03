#pragma once

#include "D3DCommon.h"
#include "D3DCore.h"
#include <array>
#include "Resources.h"

namespace Bruno
{
	class RenderPassDescriptorHeap;
	class Resource;

	class Context
	{
	public:
		Context(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType);
		virtual ~Context();

		D3D12_COMMAND_LIST_TYPE GetCommandType() { return mContextType; }
		ID3D12GraphicsCommandList* GetCommandList() { return mCommandList; }

		void AddBarrier(Resource& resource, D3D12_RESOURCE_STATES newState);
		void FlushBarriers();
		void CopyResource(const Resource& destination, const Resource& source);
		void CopyBufferRegion(Resource& destination, uint64_t destOffset, Resource& source, uint64_t sourceOffset, uint64_t numBytes);
		void CopyTextureRegion(Resource& destination, Resource& source, size_t sourceOffset, SubResourceLayouts& subResourceLayouts, uint32_t numSubResources);
		void Reset();

	protected:
		GraphicsDevice& mDevice;

		D3D12_COMMAND_LIST_TYPE mContextType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ID3D12GraphicsCommandList6* mCommandList{ nullptr };
		std::array<ID3D12CommandAllocator*, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> mCommandAllocators{ nullptr };
		std::array<D3D12_RESOURCE_BARRIER, Graphics::Core::MAX_QUEUED_BARRIERS> mResourceBarriers{};
		uint32_t mNumQueuedBarriers = 0;
		RenderPassDescriptorHeap* mCurrentSRVHeap = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE mCurrentSRVHeapHandle{ 0 };

		void BindDescriptorHeaps(uint32_t frameIndex);
	};
}
