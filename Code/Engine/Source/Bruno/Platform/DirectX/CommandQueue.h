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
		CommandQueue(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE queueType);
		~CommandQueue();

		bool IsFenceComplete(uint64_t fenceValue);
		void InsertWait(uint64_t fenceValue);
		void InsertWaitForQueueFence(CommandQueue* otherQueue, uint64_t fenceValue);
		void InsertWaitForQueue(CommandQueue* otherQueue);
		void WaitForFenceCPUBlocking(uint64_t fenceValue);
		void WaitForIdle();

		uint64_t GetLastCompletedFence() { return m_lastCompletedFenceValue; }
		uint64_t GetNextFenceValue() { return m_nextFenceValue; }
		uint64_t ExecuteCommandList(ID3D12CommandList* commandList);
		uint64_t SignalFence();

		ID3D12CommandQueue* GetQueue() { return m_commandQueue.Get(); }
		ID3D12Fence1* GetFence() { return m_fence.Get(); }

	private:
		uint64_t PollCurrentFenceValue();

		GraphicsDevice& m_device;

		D3D12_COMMAND_LIST_TYPE							m_queueType;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence1>			m_fence;
		uint64_t										m_nextFenceValue{ 1 };
		uint64_t										m_lastCompletedFenceValue{ 0 };

		HANDLE											m_fenceEventHandle{ nullptr };
		std::mutex										m_fenceMutex;
		std::mutex										m_eventMutex;
	};
}
