#pragma once

#include "D3DCore.h"

#include <mutex>

namespace Bruno
{
	struct DescriptorHandle
	{
		D3D12_CPU_DESCRIPTOR_HANDLE Cpu{};
		D3D12_GPU_DESCRIPTOR_HANDLE Gpu{};
		uint32_t					HeapIndex{ 0xffff'ffff };
	};

	class GraphicsDevice;

	class DescriptorHeap
	{
	public:
		explicit DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType) : m_heapType{ heapType } {}
		explicit DescriptorHeap(const DescriptorHeap&) = delete;
		DescriptorHeap& operator=(const DescriptorHeap&) = delete;
		explicit DescriptorHeap(DescriptorHeap&&) = delete;
		DescriptorHeap& operator=(DescriptorHeap&&) = delete;
		~DescriptorHeap();

		bool Initialize(GraphicsDevice* device, uint32_t maxDescriptors, bool isShaderVisible = false);
		constexpr D3D12_CPU_DESCRIPTOR_HANDLE GetCpuStart() const { return m_cpuStart; }
		constexpr bool IsShaderVisible() const { return m_gpuStart.ptr != 0; }
		constexpr ID3D12DescriptorHeap* const GetHeap() const { return m_heap; }

	protected:
		ID3D12DescriptorHeap*				m_heap;
		D3D12_CPU_DESCRIPTOR_HANDLE         m_cpuStart{};
		D3D12_GPU_DESCRIPTOR_HANDLE         m_gpuStart{};

		uint32_t							m_descriptorSize{ 0 };
		uint32_t							m_currentDescriptorIndex{ 0 };
		uint32_t							m_maxDescriptors{ 0 };
		const D3D12_DESCRIPTOR_HEAP_TYPE    m_heapType{};
	};

	class StagingDescriptorHeap : public DescriptorHeap
	{
	public:
		StagingDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType) : DescriptorHeap(heapType){}
		~StagingDescriptorHeap();

		DescriptorHandle Allocate(uint32_t count = 1);
		void Free(DescriptorHandle handle);

	private:
		std::vector<uint32_t> mFreeDescriptors;
		uint32_t mCurrentDescriptorIndex = 0;
		uint32_t mActiveHandleCount = 0;
	};

	class RenderPassDescriptorHeap : public DescriptorHeap
	{
	public:
		RenderPassDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType) : DescriptorHeap(heapType) {}

		DescriptorHandle Allocate();
	};
}
