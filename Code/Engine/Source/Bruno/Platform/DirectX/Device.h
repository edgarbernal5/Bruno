#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "DescriptorAllocator_Gem.h"

//#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED

namespace Bruno::DX
{
	class CommandQueue;
	
	class GraphicsDevice
	{
	public:
		GraphicsDevice();
		~GraphicsDevice() = default;

		// C++17: Previene copias accidentales del dispositivo
		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) = delete;

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Device2> GetNativeDevice() const { return m_device; }
		[[nodiscard]] Microsoft::WRL::ComPtr<IDXGIFactory4> GetDXGIFactory() const { return m_dxgiFactory; }
		
		[[nodiscard]] CommandQueue& GetDirectCommandQueue() const { return *m_directCommandQueue; }
		
		[[nodiscard]] DX::DescriptorAllocator& GetsvrDescriptorAllocator() const { return *m_svrDescriptorAllocator; }
	private:
		void InitializeDXGI();
		void CreateDevice();

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_device;
        
		// La cola principal de comandos de la GPU
		std::unique_ptr<CommandQueue> m_directCommandQueue;
		
		std::unique_ptr<DX::DescriptorAllocator> m_svrDescriptorAllocator;
	};
}