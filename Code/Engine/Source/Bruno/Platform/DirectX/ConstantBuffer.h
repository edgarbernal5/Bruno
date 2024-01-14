#pragma once

#include "GraphicsDevice.h"
#include "Bruno/Core/Memory.h"
#include "GpuBuffer.h"

namespace Bruno
{
	template<typename T>
	class ConstantBuffer : public GpuBuffer
	{
	public:
		ConstantBuffer(uint32_t elementCount = 1);
		ConstantBuffer(const ConstantBuffer& rhs) = delete;
		ConstantBuffer& operator=(const ConstantBuffer& rhs) = delete;
		//~ConstantBuffer()
		//{
		//	if (m_resource != nullptr)
		//		m_resource->Unmap(0, nullptr);

		//	m_mappedResource = nullptr;
		//}

		constexpr const uint32_t GetElementSizeInBytes() const { return m_elementSizeInBytes; }

		void SetMappedData(const T& data)
		{
			GpuBuffer::SetMappedData(&data, sizeof(T));
		}

		void SetMappedData(uint32_t elementIndexOffset, const T& data)
		{
			GpuBuffer::SetMappedData(&data, elementIndexOffset, sizeof(T));
		}

	private:
		uint32_t m_elementSizeInBytes;
	};

	template<typename T>
	inline ConstantBuffer<T>::ConstantBuffer(uint32_t elementCount) :
		GpuBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(AlignU32(sizeof(T), 256) * elementCount, AlignU32(sizeof(T), 256), BufferViewFlags::Cbv, BufferAccessFlags::HostWritable))
	{
		m_elementSizeInBytes = AlignU32(sizeof(T), 256);
	}
}

