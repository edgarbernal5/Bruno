#pragma once

#include "D3DCommon.h"
#include "GraphicsDevice.h"
#include "Bruno/Core/Memory.h"
#include "GPUBuffer.h"

namespace Bruno
{
	template<typename T>
	class ConstantBuffer : public GPUBuffer
	{
	public:
		ConstantBuffer();
		ConstantBuffer(const ConstantBuffer& rhs) = delete;
		ConstantBuffer& operator=(const ConstantBuffer& rhs) = delete;
		//~ConstantBuffer()
		//{
		//	if (m_resource != nullptr)
		//		m_resource->Unmap(0, nullptr);

		//	m_mappedResource = nullptr;
		//}

		constexpr const uint32_t GetSizeInBytes() const { return m_elementSizeInBytes; }

		void SetMappedData(const T& data)
		{
			GPUBuffer::SetMappedData(&data, sizeof(T));
		}

	private:
		uint32_t m_elementSizeInBytes;
	};

	template<typename T>
	inline ConstantBuffer<T>::ConstantBuffer() :
		GPUBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(AlignU32(sizeof(T), 256), BufferViewFlags::Cbv, BufferAccessFlags::HostWritable))
	{
		m_elementSizeInBytes = AlignU32(sizeof(T), 256);
	}
}

