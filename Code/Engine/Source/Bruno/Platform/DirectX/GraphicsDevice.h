#pragma once

#include "D3DCommon.h"
#include "D3DCore.h"
#include "Resources.h"

#include "CommandQueue.h"
#include <array>

namespace D3D12MA
{
	class Allocator;
	class Allocation;
}

namespace Bruno
{
	class GraphicsAdapter;
	class CommandQueue;
	class Context;
	class GraphicsContext;
	class UploadContext;
	class Surface;

    struct ContextSubmissionResult
    {
        uint32_t mFrameId = 0;
        uint32_t mSubmissionIndex = 0;
    };

	class GraphicsDevice
	{
	public:
		GraphicsDevice(std::shared_ptr<GraphicsAdapter> adapter = nullptr);
		~GraphicsDevice();

		void BeginFrame();
		void EndFrame();
		void Present(Surface* surface);

		void CopyDescriptors(uint32_t numDestDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* destDescriptorRangeStarts, const uint32_t* destDescriptorRangeSizes,
			uint32_t numSrcDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* srcDescriptorRangeStarts, const uint32_t* srcDescriptorRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE descriptorType);
		void CopySRVHandleToReservedTable(DescriptorHandle srvHandle, uint32_t index);

		uint32_t GetFrameId() { return m_frameId; }

		IDXGIFactory4* GetFactory();
		ID3D12Device5* GetD3DDevice();
		CommandQueue* GetCommandQueue();
		//GraphicsContext& GetGraphicsContext();

		StagingDescriptorHeap& GetRtvDescriptionHeap();
		StagingDescriptorHeap& GetDsvDescriptionHeap();
		StagingDescriptorHeap& GetSrvDescriptionHeap();

		RenderPassDescriptorHeap& GetSrvDescriptionHeap(uint32_t frameIndex) { return *mSRVRenderPassDescriptorHeaps[frameIndex]; }
		RenderPassDescriptorHeap& GetSamplerHeap() { return *mSamplerRenderPassDescriptorHeap; }
		UploadContext& GetUploadContext() { return *mUploadContexts[m_frameId]; }

		D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() const { return m_highestRootSignatureVersion; }

		D3D12MA::Allocator* GetAllocator() const;

		uint32_t GetFreeReservedDescriptorIndex();

		ContextSubmissionResult SubmitContextWork(Context& context);
		void WaitForIdle();

		static std::shared_ptr<GraphicsDevice> Create(std::shared_ptr<GraphicsAdapter> adapter = nullptr);

	private:
		struct EndOfFrameFences
		{
			uint64_t mGraphicsQueueFence = 0;
			uint64_t mComputeQueueFence = 0;
			uint64_t mCopyQueueFence = 0;
		};

		uint32_t											m_frameId{ 0 };
		Microsoft::WRL::ComPtr<ID3D12Resource>              m_depthStencil;

		std::shared_ptr<GraphicsAdapter>					m_adapter;

		Microsoft::WRL::ComPtr<ID3D12Device5>               m_d3dDevice;
		std::unique_ptr<CommandQueue>						mGraphicsQueue;
		std::unique_ptr<CommandQueue>						mComputeQueue;
		std::unique_ptr<CommandQueue>						mCopyQueue;

		D3D_FEATURE_LEVEL									m_d3dMinFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };
		Microsoft::WRL::ComPtr<IDXGIFactory4>               m_dxgiFactory;
		DWORD                                               m_dxgiFactoryFlags{ 0 };
		D3D_FEATURE_LEVEL                                   m_d3dFeatureLevel;

		std::unique_ptr<StagingDescriptorHeap>				m_rtvDescriptorHeap;// { D3D12_DESCRIPTOR_HEAP_TYPE_RTV };
		std::unique_ptr<StagingDescriptorHeap>				m_dsvDescriptorHeap;// { D3D12_DESCRIPTOR_HEAP_TYPE_DSV };
		std::unique_ptr<StagingDescriptorHeap>				m_srvDescriptorHeap;// { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };
		std::unique_ptr<RenderPassDescriptorHeap>			mSamplerRenderPassDescriptorHeap;
		std::array<std::unique_ptr<RenderPassDescriptorHeap>, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> mSRVRenderPassDescriptorHeaps;

		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		D3D_ROOT_SIGNATURE_VERSION							m_highestRootSignatureVersion;
		D3D12MA::Allocator*									mAllocator = nullptr;
		std::vector<uint32_t>								mFreeReservedDescriptorIndices;


		std::array<std::vector<std::pair<uint64_t, D3D12_COMMAND_LIST_TYPE>>, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> mContextSubmissions;

		std::array<std::unique_ptr<UploadContext>, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> mUploadContexts;
		std::array<EndOfFrameFences, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> mEndOfFrameFences;

		void ProcessDestructions(uint32_t frameIndex);
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