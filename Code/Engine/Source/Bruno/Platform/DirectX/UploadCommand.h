#pragma once

#include "D3DCore.h"

namespace Bruno
{
	class GraphicsDevice;
	class Surface;

	class UploadCommand
	{
	public:
		UploadCommand(GraphicsDevice* device);
		~UploadCommand();

		void BeginUpload(uint32_t alignedSize);
		void EndUpload();
		void Flush();

		ID3D12CommandQueue* GetQueue();
		//ID3D12GraphicsCommandList6* GetCommandList();

	private:
		struct UploadFrame
		{
			ID3D12CommandAllocator*		CommandAllocator{ nullptr };
			ID3D12GraphicsCommandList6*	CommandList{ nullptr };
			ID3D12Resource*				UploadBuffer{ nullptr };
			uint64_t					FenceValue{ 0 };

			void Release(ID3D12Fence1* uploadFence, HANDLE fenceEvent)
			{
				WaitAndReset(uploadFence, fenceEvent);
				if (CommandAllocator)
				{
					CommandAllocator->Release();
					CommandAllocator = nullptr;
				}
				if (CommandList)
				{
					CommandList->Release();
					CommandList = nullptr;
				}
				FenceValue = 0;
			}

			void WaitAndReset(ID3D12Fence1* uploadFence, HANDLE fenceEvent)
			{
				if (uploadFence->GetCompletedValue() < FenceValue)
				{
					ThrowIfFailed(uploadFence->SetEventOnCompletion(FenceValue, fenceEvent));
					WaitForSingleObject(fenceEvent, INFINITE);
				}
				if (UploadBuffer)
				{
					UploadBuffer->Release();
					UploadBuffer = nullptr;
				}
			}
		};

		const D3D12_HEAP_PROPERTIES DefaultHeap {
			D3D12_HEAP_TYPE_DEFAULT,                        // Type
			D3D12_CPU_PAGE_PROPERTY_UNKNOWN,                // CPUPageProperty
			D3D12_MEMORY_POOL_UNKNOWN,                      // MemoryPoolPreference
			0,                                              // CreationNodeMask
			0                                               // VisibleNodeMask
		};

		const D3D12_HEAP_PROPERTIES UploadHeap {
			D3D12_HEAP_TYPE_UPLOAD,                         // Type
			D3D12_CPU_PAGE_PROPERTY_UNKNOWN,                // CPUPageProperty
			D3D12_MEMORY_POOL_UNKNOWN,                      // MemoryPoolPreference
			0,                                              // CreationNodeMask
			0                                               // VisibleNodeMask
		};

		GraphicsDevice*									m_device;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_uploadCommand;
		Microsoft::WRL::ComPtr<ID3D12Fence1>			m_fence;
		uint64_t										m_fenceValue{ 0 };
		
		UploadFrame										m_uploadFrames[Graphics::Core::UPLOAD_FRAME_COUNT];
		HANDLE											m_fenceEvent{ nullptr };
		uint32_t										m_frameIndex{ 0 };
	};
}
