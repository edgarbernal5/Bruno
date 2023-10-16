#include "brpch.h"
#include "CommandQueue.h"

#include "GraphicsDevice.h"
#include "Surface.h"

namespace Bruno
{
	CommandQueue::CommandQueue(GraphicsDevice* device, D3D12_COMMAND_LIST_TYPE queueType) :
		m_device(device),
		m_queueType(queueType)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask	= 0;
		desc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type		= queueType;

		ThrowIfFailed(device->GetD3DDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));
		
		switch (queueType)
		{
		case D3D12_COMMAND_LIST_TYPE_COPY:
			m_commandQueue->SetName(L"Copy Command Queue");
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			m_commandQueue->SetName(L"Compute Command Queue");
			break;
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			m_commandQueue->SetName(L"Direct Command Queue");
			break;
		}

		ThrowIfFailed(m_device->GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fence->SetName(L"CommandQueue");

		m_fence->Signal(m_lastCompletedFenceValue);

		m_fenceEventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		BR_ASSERT(m_fenceEventHandle != INVALID_HANDLE_VALUE);
	}

	CommandQueue::~CommandQueue()
	{
		CloseHandle(m_fenceEventHandle);
		m_fenceEventHandle = nullptr;

		
	}

	bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
	{
		if (fenceValue > m_lastCompletedFenceValue)
		{
			PollCurrentFenceValue();
		}

		return fenceValue <= m_lastCompletedFenceValue;
	}

	void CommandQueue::InsertWait(uint64_t fenceValue)
	{
		m_commandQueue->Wait(m_fence.Get(), fenceValue);
	}

	void CommandQueue::InsertWaitForQueueFence(CommandQueue* otherQueue, uint64_t fenceValue)
	{
		m_commandQueue->Wait(otherQueue->GetFence(), fenceValue);
	}

	void CommandQueue::InsertWaitForQueue(CommandQueue* otherQueue)
	{
		m_commandQueue->Wait(otherQueue->GetFence(), otherQueue->GetNextFenceValue() - 1);
	}

	void CommandQueue::WaitForFenceCPUBlocking(uint64_t fenceValue)
	{
		if (IsFenceComplete(fenceValue))
		{
			return;
		}

		{
			std::lock_guard<std::mutex> lockGuard(m_eventMutex);

			m_fence->SetEventOnCompletion(fenceValue, m_fenceEventHandle);
			WaitForSingleObjectEx(m_fenceEventHandle, INFINITE, false);
			m_lastCompletedFenceValue = fenceValue;
		}
	}

	void CommandQueue::WaitForIdle()
	{
		WaitForFenceCPUBlocking(m_nextFenceValue - 1);
	}

	uint64_t CommandQueue::PollCurrentFenceValue()
	{
		m_lastCompletedFenceValue = (std::max)(m_lastCompletedFenceValue, m_fence->GetCompletedValue());
		return m_lastCompletedFenceValue;
	}

	uint64_t CommandQueue::ExecuteCommandList(ID3D12CommandList* commandList)
	{
		ThrowIfFailed(static_cast<ID3D12GraphicsCommandList*>(commandList)->Close());
		m_commandQueue->ExecuteCommandLists(1, &commandList);

		return SignalFence();
	}

	uint64_t CommandQueue::SignalFence()
	{
		std::lock_guard<std::mutex> lockGuard(m_fenceMutex);

		m_commandQueue->Signal(m_fence.Get(), m_nextFenceValue);

		return m_nextFenceValue++;
	}
}