#pragma once

#include <array>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "D3DConstants.h"

//#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED

namespace Bruno
{
	class CommandQueue 
	{
	public:
		CommandQueue(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();

		CommandQueue(const CommandQueue&) = delete;
		CommandQueue& operator=(const CommandQueue&) = delete;

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetNativeQueue() const { return m_commandQueue; }
        
		// FIRMAS CORREGIDAS: Reciben en qué frame estamos trabajando
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList(uint32_t frameIndex);
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetAllocator(uint32_t frameIndex);
		
		uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, uint32_t frameIndex);
        
		void Flush();

	private:
		void WaitForFenceValue(uint64_t fenceValue);

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
        
		// EL SECRETO AAA: Un Allocator por cada frame del SwapChain
		std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, Graphics::Core::BACK_BUFFER_COUNT> m_commandAllocators;
		// Solo necesitamos UN lápiz (CommandList) porque la CPU solo escribe un frame a la vez
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		// Sistema de Sincronización
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		uint64_t m_fenceValue;
		HANDLE m_fenceEvent;

		// El ticket en el que terminó cada frame individual
		uint64_t m_frameFenceValues[Graphics::Core::BACK_BUFFER_COUNT];
	};
}