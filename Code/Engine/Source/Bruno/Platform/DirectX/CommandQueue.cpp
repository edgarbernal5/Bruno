#include "brpch.h"
#include "CommandQueue.h"

#include "GraphicsDevice.h"
#include "Surface.h"

namespace Bruno
{
	CommandQueue::CommandQueue(GraphicsDevice* device, D3D12_COMMAND_LIST_TYPE type) :
		m_device(device)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask	= 0;
		desc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type		= type;

		ThrowIfFailed(device->GetD3DDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));
		
		switch (type)
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

		for (uint32_t i = 0; i < Graphics::Core::FRAME_BUFFER_COUNT; i++)
		{
			CommandFrame& frame = m_commandFrames[i];
			ThrowIfFailed(m_device->GetD3DDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&frame.CommandAllocator)));

			wchar_t name[25] = {};
			swprintf_s(name, L"Render target %u", i);
			frame.CommandAllocator->SetName(name);
		}

		ThrowIfFailed(m_device->GetD3DDevice()->CreateCommandList(0, type, m_commandFrames[0].CommandAllocator, nullptr, IID_PPV_ARGS(&m_commandList)));
		m_commandList->Close();
		m_commandList->SetName(L"CommandQueue");

		ThrowIfFailed(m_device->GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fence->SetName(L"CommandQueue");

		m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	}

	CommandQueue::~CommandQueue()
	{
		Flush();

		CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;

		for (uint32_t i = 0; i < Graphics::Core::FRAME_BUFFER_COUNT; ++i)
		{
			m_commandFrames[i].Release();
		}

		m_fenceValue = 0;
	}

	void CommandQueue::BeginFrame()
	{
		CommandFrame& frame{ m_commandFrames[m_frameIndex] };
		frame.Wait(m_fenceEvent, m_fence.Get());
		ThrowIfFailed(frame.CommandAllocator->Reset());
		ThrowIfFailed(m_commandList->Reset(frame.CommandAllocator, nullptr));
	}

	void CommandQueue::EndFrame(Surface* surface)
	{
		ThrowIfFailed(m_commandList->Close());
		ID3D12CommandList* const commandLists[]{ m_commandList.Get()};
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), &commandLists[0]);

		// Presenting swap chain buffers happens in lockstep with frame buffers.
		surface->Present();

		uint64_t& currentFenceValue{ m_fenceValue };
		++currentFenceValue;
		CommandFrame& frame{ m_commandFrames[m_frameIndex] };
		frame.FenceValue = currentFenceValue;
		m_commandQueue->Signal(m_fence.Get(), currentFenceValue);

		m_frameIndex = (m_frameIndex + 1) % Graphics::Core::FRAME_BUFFER_COUNT;
	}

	void CommandQueue::Flush()
	{
		for (uint32_t i = 0; i < Graphics::Core::FRAME_BUFFER_COUNT; ++i)
		{
			m_commandFrames[i].Wait(m_fenceEvent, m_fence.Get());
		}
		m_frameIndex = 0;
	}

	ID3D12CommandQueue* CommandQueue::GetQueue()
	{
		return m_commandQueue.Get();
	}

	ID3D12GraphicsCommandList6* CommandQueue::GetCommandList()
	{
		return m_commandList.Get();
	}
}