#pragma once

#include "D3DCore.h"

#include <mutex>

namespace Bruno
{
	struct DescriptorHandle
	{
		D3D12_CPU_DESCRIPTOR_HANDLE Cpu{};
		D3D12_GPU_DESCRIPTOR_HANDLE Gpu{};
		uint32_t					Index{ 0xffff'ffff };
	};

	class GraphicsDevice;

	class DescriptorHeap
	{
	public:
		explicit DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) : m_type{ type } {}
		explicit DescriptorHeap(const DescriptorHeap&) = delete;
		DescriptorHeap& operator=(const DescriptorHeap&) = delete;
		explicit DescriptorHeap(DescriptorHeap&&) = delete;
		DescriptorHeap& operator=(DescriptorHeap&&) = delete;

		DescriptorHandle Allocate();
		bool Initialize(GraphicsDevice* device, uint32_t capacity, bool isShaderVisible = false);
		constexpr D3D12_CPU_DESCRIPTOR_HANDLE GetCpuStart() const { return m_cpuStart; }
		constexpr bool IsShaderVisible() const { return m_gpuStart.ptr != 0; }
		constexpr ID3D12DescriptorHeap* const GetHeap() const { return m_heap; }

	private:
		ID3D12DescriptorHeap*				m_heap;
		D3D12_CPU_DESCRIPTOR_HANDLE         m_cpuStart{};
		D3D12_GPU_DESCRIPTOR_HANDLE         m_gpuStart{};

		uint32_t							m_descriptorSize{0};
		uint32_t							m_size{0};
		uint32_t							m_capacity{ 0 };
		const D3D12_DESCRIPTOR_HEAP_TYPE    m_type{};
	};
}
