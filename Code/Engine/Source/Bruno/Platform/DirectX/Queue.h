#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

//#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED

namespace Bruno::DX
{
	class CommandQueue {
	public:
		CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetNativeQueue() const { return m_commandQueue; }
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList();

		// Ejecuta la lista y devuelve un "Ticket" (Fence Value)
		uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

		// Sincronización: Detiene la CPU hasta que la GPU alcance este ticket
		void WaitForFenceValue(uint64_t fenceValue);
        
		// Sincronización Total (Ideal para el ResizeWindow)
		void Flush();

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
        
		// --- Sistema de Sincronización (El semáforo CPU/GPU) ---
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		uint64_t m_fenceValue;
		HANDLE m_fenceEvent;
	};
}