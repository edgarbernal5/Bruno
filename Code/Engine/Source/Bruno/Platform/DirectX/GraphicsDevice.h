#pragma once

#include "D3DCommon.h"
#include "D3DCore.h"
#include "Resources.h"

#include "CommandQueue.h"

namespace Bruno
{
	class GraphicsAdapter;
	class CommandQueue;
	class UploadCommand;

	class GraphicsDevice
	{
	public:
		GraphicsDevice(std::shared_ptr<GraphicsAdapter> adapter = nullptr);
		~GraphicsDevice() = default;

		IDXGIFactory4* GetFactory();
		ID3D12Device2* GetD3DDevice();
		CommandQueue* GetCommandQueue();
		UploadCommand* GetUploadCommand();
		DescriptorHeap& GetRtvDescriptionHeap(); 
		DescriptorHeap& GetDsvDescriptionHeap();
		DescriptorHeap& GetSrvDescriptionHeap();
		
		D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() const
		{
			return m_highestRootSignatureVersion;
		}

		static std::shared_ptr<GraphicsDevice> Create(std::shared_ptr<GraphicsAdapter> adapter = nullptr);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>              m_depthStencil;

		std::shared_ptr<GraphicsAdapter>					m_adapter;

		Microsoft::WRL::ComPtr<ID3D12Device2>               m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      m_commandAllocators[Graphics::Core::FRAME_BUFFER_COUNT];
		std::unique_ptr<CommandQueue>						m_commandQueue;
		std::unique_ptr<UploadCommand>						m_uploadCommand;

		D3D_FEATURE_LEVEL									m_d3dMinFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };
		Microsoft::WRL::ComPtr<IDXGIFactory4>               m_dxgiFactory;
		DWORD                                               m_dxgiFactoryFlags{ 0 };
		D3D_FEATURE_LEVEL                                   m_d3dFeatureLevel;

		DescriptorHeap										m_rtvDescriptorHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV };
		DescriptorHeap										m_dsvDescriptorHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV };
		DescriptorHeap										m_srvDescriptorHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };

		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		D3D_ROOT_SIGNATURE_VERSION							m_highestRootSignatureVersion;
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