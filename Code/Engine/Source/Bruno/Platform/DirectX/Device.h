#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

//#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED

namespace Bruno::DX
{
	class GraphicsDevice 
	{
	public:
		GraphicsDevice();
		~GraphicsDevice() = default;

		// C++17: Previene copias accidentales del dispositivo (Bug común)
		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) = delete;

		[[nodiscard]] ID3D12Device* GetNativeDevice() const { return m_device.Get(); }
		[[nodiscard]] ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }
		[[nodiscard]] IDXGIFactory4* GetFactory() const { return m_factory.Get(); }

	private:
		void EnableDebugLayer();
		void CreateDevice();
		void CreateCommandQueue();

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	};
}