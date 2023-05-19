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
		constexpr D3D12_CPU_DESCRIPTOR_HANDLE GetCpuStart() const { return m_cpu_start; }

	private:
		ID3D12DescriptorHeap*				m_heap;
		D3D12_CPU_DESCRIPTOR_HANDLE         m_cpu_start{};
		D3D12_GPU_DESCRIPTOR_HANDLE         m_gpu_start{};

		uint32_t							m_descriptor_size{0};
		uint32_t							m_size{0};
		const D3D12_DESCRIPTOR_HEAP_TYPE    m_type{};
	};
}
