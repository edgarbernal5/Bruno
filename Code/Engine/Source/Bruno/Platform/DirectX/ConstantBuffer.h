#pragma once

#include "D3DCommon.h"
#include "GraphicsDevice.h"

namespace Bruno
{
	template<typename T>
	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		ConstantBuffer(const ConstantBuffer& rhs) = delete;
		ConstantBuffer& operator=(const ConstantBuffer& rhs) = delete;
		~ConstantBuffer()
		{
			if (m_d3dBuffer != nullptr)
				m_d3dBuffer->Unmap(0, nullptr);

			m_mappedData = nullptr;
		}

		constexpr ID3D12Resource* GetResource() const { return m_d3dBuffer.Get(); }
		constexpr const uint32_t GetSizeInBytes() const { return m_elementSizeInBytes; }

		void CopyData(const T& data)
		{
			memcpy(m_mappedData, &data, sizeof(T));
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_d3dBuffer;
		uint32_t m_elementSizeInBytes;
		uint8_t* m_mappedData = nullptr;

		uint32_t GetAlignedConstantBufferSize(uint32_t sizeInBytes);
	};

	template<typename T>
	inline ConstantBuffer<T>::ConstantBuffer()
	{
		m_elementSizeInBytes = GetAlignedConstantBufferSize(sizeof(T));

		auto device = Graphics::GetDevice();
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_elementSizeInBytes);
		ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
			&Graphics::Core::HeapProperties.UploadHeap,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_d3dBuffer)));

		ThrowIfFailed(m_d3dBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData))); // m_mappedData = cpu_address
	}

	template<typename T>
	inline uint32_t ConstantBuffer<T>::GetAlignedConstantBufferSize(uint32_t sizeInBytes)
	{
		return (sizeInBytes + 255) & ~255;
	}
}

