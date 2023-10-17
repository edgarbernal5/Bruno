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
	class PipelineStateObject;
	class GPUBuffer;
	class Texture;

    struct ContextSubmissionResult
    {
        uint32_t FrameId = 0;
        uint32_t SubmissionIndex = 0;
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

		void DestroyTexture(std::unique_ptr<Texture> texture);
		uint32_t GetFrameId() { return m_frameId; }

		IDXGIFactory4* GetFactory();
		ID3D12Device5* GetD3DDevice();
		CommandQueue* GetGraphicsQueue() { return m_graphicsQueue.get(); }

		StagingDescriptorHeap& GetRtvDescriptionHeap();
		StagingDescriptorHeap& GetDsvDescriptionHeap();
		StagingDescriptorHeap& GetSrvDescriptionHeap();

		RenderPassDescriptorHeap& GetSrvDescriptionHeap(uint32_t frameIndex) { return *m_srvRenderPassDescriptorHeaps[frameIndex]; }
		RenderPassDescriptorHeap& GetSamplerHeap() { return *m_samplerRenderPassDescriptorHeap; }
		UploadContext& GetUploadContext() { return *m_uploadContexts[m_frameId]; }

		D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() const { return m_highestRootSignatureVersion; }

		D3D12MA::Allocator* GetAllocator() const;

		uint32_t GetFreeReservedDescriptorIndex();

		ContextSubmissionResult SubmitContextWork(Context& context);
		void WaitForIdle();

		static std::shared_ptr<GraphicsDevice> Create(std::shared_ptr<GraphicsAdapter> adapter = nullptr);

	private:
		struct EndOfFrameFences
		{
			uint64_t GraphicsQueueFence = 0;
			uint64_t ComputeQueueFence = 0;
			uint64_t CopyQueueFence = 0;
		};

		struct DestructionQueue
		{
			std::vector<std::unique_ptr<GPUBuffer>> BuffersToDestroy;
			std::vector<std::unique_ptr<Texture>> TexturesToDestroy;
			std::vector<std::unique_ptr<PipelineStateObject>> PipelinesToDestroy;
			std::vector<std::unique_ptr<Context>> ContextsToDestroy;
		};

		void CopyDescriptorsSimple(uint32_t numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE descriptorType);
		void ProcessDestructions(uint32_t frameIndex);

		uint32_t											m_frameId{ 0 };
		Microsoft::WRL::ComPtr<ID3D12Resource>              m_depthStencil;

		std::shared_ptr<GraphicsAdapter>					m_adapter;

		Microsoft::WRL::ComPtr<ID3D12Device5>               m_d3dDevice;
		std::unique_ptr<CommandQueue>						m_graphicsQueue;
		std::unique_ptr<CommandQueue>						m_computeQueue;
		std::unique_ptr<CommandQueue>						m_copyQueue;

		D3D_FEATURE_LEVEL									m_d3dMinFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };
		Microsoft::WRL::ComPtr<IDXGIFactory4>               m_dxgiFactory;
		DWORD                                               m_dxgiFactoryFlags{ 0 };
		D3D_FEATURE_LEVEL                                   m_d3dFeatureLevel;

		std::unique_ptr<StagingDescriptorHeap>				m_rtvDescriptorHeap;// { D3D12_DESCRIPTOR_HEAP_TYPE_RTV };
		std::unique_ptr<StagingDescriptorHeap>				m_dsvDescriptorHeap;// { D3D12_DESCRIPTOR_HEAP_TYPE_DSV };
		std::unique_ptr<StagingDescriptorHeap>				m_srvDescriptorHeap;// { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };
		std::unique_ptr<RenderPassDescriptorHeap>			m_samplerRenderPassDescriptorHeap;
		std::array<std::unique_ptr<RenderPassDescriptorHeap>, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> m_srvRenderPassDescriptorHeaps;

		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		D3D_ROOT_SIGNATURE_VERSION							m_highestRootSignatureVersion;
		D3D12MA::Allocator*									m_allocator = nullptr;
		std::vector<uint32_t>								m_freeReservedDescriptorIndices;

		std::array<std::vector<std::pair<uint64_t, D3D12_COMMAND_LIST_TYPE>>, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> m_contextSubmissions;

		std::array<std::unique_ptr<UploadContext>, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> m_uploadContexts;
		std::array<EndOfFrameFences, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> m_endOfFrameFences;
		std::array<DestructionQueue, Graphics::Core::FRAMES_IN_FLIGHT_COUNT> m_destructionQueues;
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