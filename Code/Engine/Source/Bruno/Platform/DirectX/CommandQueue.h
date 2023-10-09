#pragma once

#include "D3DCore.h"
#include <mutex>

namespace Bruno
{
	class GraphicsDevice;
	class Surface;

	class CommandQueue
	{
	public:
		CommandQueue(GraphicsDevice* device, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();

		bool IsFenceComplete(uint64_t fenceValue);
		void InsertWait(uint64_t fenceValue);
		void InsertWaitForQueueFence(CommandQueue* otherQueue, uint64_t fenceValue);
		void InsertWaitForQueue(CommandQueue* otherQueue);
		void WaitForFenceCPUBlocking(uint64_t fenceValue);
		void WaitForIdle();

		uint64_t GetLastCompletedFence() { return mLastCompletedFenceValue; }
		uint64_t GetNextFenceValue() { return mNextFenceValue; }
		uint64_t ExecuteCommandList(ID3D12CommandList* commandList);
		uint64_t SignalFence();

		ID3D12CommandQueue* GetQueue() { return m_commandQueue.Get(); }
		ID3D12Fence1* GetFence() { return m_fence.Get(); }

	private:
		GraphicsDevice*									m_device;

		D3D12_COMMAND_LIST_TYPE							m_queueType;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence1>			m_fence;
		uint64_t										mNextFenceValue { 1 };
		uint64_t										mLastCompletedFenceValue{ 0 };
		
		HANDLE											m_fenceEventHandle{ nullptr };
		uint32_t										m_frameIndex{ 0 };
		std::mutex										mFenceMutex;
		std::mutex										mEventMutex;

		uint64_t PollCurrentFenceValue();
	};
}
