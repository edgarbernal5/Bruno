#pragma once

#include "D3DCommon.h"
#include "D3DCore.h"
#include "Resources.h"

#include "CommandQueue.h"

namespace Bruno
{
	class GraphicsAdapter;
	class CommandQueue;

	class GraphicsDevice
	{
	public:
		GraphicsDevice(std::shared_ptr<GraphicsAdapter> adapter = nullptr);
		~GraphicsDevice() = default;

		IDXGIFactory4* GetFactory();
		ID3D12Device* GetD3DDevice();
		CommandQueue* GetCommandQueue();
		DescriptorHeap& GetRtvDescriptionHeap();

		static std::shared_ptr<GraphicsDevice> Create(std::shared_ptr<GraphicsAdapter> adapter = nullptr);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>              m_depthStencil;

		std::shared_ptr<GraphicsAdapter>					m_adapter;

		Microsoft::WRL::ComPtr<ID3D12Device>                m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      m_commandAllocators[Graphics::Core::FRAME_BUFFER_COUNT];
		std::unique_ptr<CommandQueue>						m_commandQueue;

		D3D_FEATURE_LEVEL									m_d3dMinFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };
		Microsoft::WRL::ComPtr<IDXGIFactory4>               m_dxgiFactory;
		DWORD                                               m_dxgiFactoryFlags{ 0 };
		D3D_FEATURE_LEVEL                                   m_d3dFeatureLevel;

		DescriptorHeap										m_rtvDescriptorHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV };

		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
	};

}

namespace Bruno::Graphics
{
	inline GraphicsDevice*& GetDevice()
	{
		static GraphicsDevice* g_device = nullptr;
		return g_device;
	}
}