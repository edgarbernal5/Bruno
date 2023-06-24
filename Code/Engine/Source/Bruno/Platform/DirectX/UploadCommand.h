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

		void BeginUpload(uint64_t bufferSize);
		Microsoft::WRL::ComPtr<ID3D12Resource> Update(const void* bufferData, uint64_t bufferSize);
		Microsoft::WRL::ComPtr<ID3D12Resource> Update(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_SUBRESOURCE_DATA* subresourcesData, uint32_t firstSubresource, uint32_t numSubresources);
		void EndUpload();
		void Flush();

		ID3D12CommandQueue* GetQueue();

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

		GraphicsDevice*									m_device;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_uploadCommand;
		Microsoft::WRL::ComPtr<ID3D12Fence1>			m_fence;
		uint64_t										m_fenceValue{ 0 };
		
		UploadFrame										m_uploadFrames[Graphics::Core::UPLOAD_FRAME_COUNT];
		HANDLE											m_fenceEvent{ nullptr };
		uint32_t										m_frameIndex{ 0 };
	};
}
