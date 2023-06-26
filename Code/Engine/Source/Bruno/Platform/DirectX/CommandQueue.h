#pragma once

#include "D3DCore.h"

namespace Bruno
{
	class GraphicsDevice;
	class Surface;

	class CommandQueue
	{
	public:
		CommandQueue(GraphicsDevice* device, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();

		void BeginFrame();
		void EndFrame(Surface* surface);
		void Flush();
		void WaitFrame();

		constexpr uint32_t GetFrameIndex() const { return m_frameIndex; }

		ID3D12CommandQueue* GetQueue();
		ID3D12GraphicsCommandList6* GetCommandList();

	private:
		struct CommandFrame
		{
			ID3D12CommandAllocator*		CommandAllocator{ nullptr };
			uint64_t					FenceValue{ 0 };

			void Wait(HANDLE fenceEvent, ID3D12Fence1* fence)
			{
				// If the current fence value is still less than "FenceValue"
				// then we know the GPU has not finished executing the command lists
				// since it has not reached the "_commandQueue->Signal()" command
				if (fence->GetCompletedValue() < FenceValue)
				{
					// We have the fence create an event wich is singaled once the fence's current value equals "FenceValue"
					ThrowIfFailed(fence->SetEventOnCompletion(FenceValue, fenceEvent));
					// Wait until the fence has triggered the event that its current value has reached "FenceValue"
					// indicating that command queue has finished executing.
					WaitForSingleObject(fenceEvent, INFINITE);
				}
			}

			void Release()
			{
				if (CommandAllocator)
				{
					CommandAllocator->Release();
					CommandAllocator = nullptr;
				}
				FenceValue = 0;
			}
		};

		GraphicsDevice*											m_device;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue>				m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>		m_commandList;
		Microsoft::WRL::ComPtr<ID3D12Fence1>					m_fence;
		uint64_t												m_fenceValue{ 0 };
		
		CommandFrame											m_commandFrames[Graphics::Core::FRAME_BUFFER_COUNT];
		HANDLE													m_fenceEvent{ nullptr };
		uint32_t												m_frameIndex{ 0 };
	};
}
