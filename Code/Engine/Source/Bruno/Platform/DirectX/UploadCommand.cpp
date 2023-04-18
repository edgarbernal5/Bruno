#include "brpch.h"
#include "UploadCommand.h"

#include "GraphicsDevice.h"
#include "Surface.h"

namespace Bruno
{
	UploadCommand::UploadCommand(GraphicsDevice* device) :
		m_device(device)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask	= 0;
		desc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type		= D3D12_COMMAND_LIST_TYPE_COPY;

		ThrowIfFailed(device->GetD3DDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_uploadCommand)));
		m_uploadCommand->SetName(L"Upload Command Queue");

		for (size_t i = 0; i < Graphics::Core::UPLOAD_FRAME_COUNT; i++)
		{
			UploadFrame& frame = m_uploadFrames[i];
			ThrowIfFailed(m_device->GetD3DDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&frame.CommandAllocator)));
			frame.CommandAllocator->SetName(L"Upload Command Allocator");

			ThrowIfFailed(m_device->GetD3DDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_uploadFrames[i].CommandAllocator, nullptr, IID_PPV_ARGS(&frame.CommandList)));
			ThrowIfFailed(frame.CommandList->Close());
			frame.CommandList->SetName(L"Upload Command List");
		}
			
		ThrowIfFailed(m_device->GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fence->SetName(L"Upload Fence");

		m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	}

	UploadCommand::~UploadCommand()
	{
		Flush();

		CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;

		for (uint32_t i = 0; i < Graphics::Core::UPLOAD_FRAME_COUNT; ++i)
		{
			m_uploadFrames[i].Release(m_fence.Get(), m_fenceEvent);
		}

		m_fenceValue = 0;
	}

	void UploadCommand::BeginUpload(uint32_t aligned_size)
	{
		UploadFrame& frame{ m_uploadFrames[m_frameIndex] };

		{//D3D12_RESOURCE_FLAG_NONE
			auto desc = CD3DX12_RESOURCE_DESC::Buffer(aligned_size);
			ID3D12Resource* intermediateBuffer;
			m_device->GetD3DDevice()->CreateCommittedResource(
				&UploadHeap,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, 
				IID_PPV_ARGS(&intermediateBuffer));

			frame.UploadBuffer = intermediateBuffer;
		}
		//UpdateSubresources
		ThrowIfFailed(frame.CommandAllocator->Reset());
		ThrowIfFailed(frame.CommandList->Reset(frame.CommandAllocator, nullptr));
	}

	void UploadCommand::EndUpload()
	{
		UploadFrame& frame{ m_uploadFrames[m_frameIndex] };
		ThrowIfFailed(frame.CommandList->Close());

		ID3D12CommandList* const commandLists[]{ frame.CommandList };
		m_uploadCommand->ExecuteCommandLists(_countof(commandLists), &commandLists[0]);

		uint64_t& currentFenceValue{ m_fenceValue };
		++currentFenceValue;
		frame.FenceValue = currentFenceValue;
		m_uploadCommand->Signal(m_fence.Get(), currentFenceValue);

		m_frameIndex = (m_frameIndex + 1) % Graphics::Core::UPLOAD_FRAME_COUNT;
	}

	void UploadCommand::Flush()
	{
		for (uint32_t i = 0; i < Graphics::Core::UPLOAD_FRAME_COUNT; ++i)
		{
			//m_uploadFrames[i].WaitAndReset(m_fenceEvent, m_fence.Get());
		}
		m_frameIndex = 0;
	}

	ID3D12CommandQueue* UploadCommand::GetQueue()
	{
		return m_uploadCommand.Get();
	}

	//ID3D12GraphicsCommandList6* UploadCommand::GetCommandList()
	//{
	//	return m_commandList.Get();
	//}
}