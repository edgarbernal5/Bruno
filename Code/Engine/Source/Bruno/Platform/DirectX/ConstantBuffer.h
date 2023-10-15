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
		~ConstantBuffer()
		{
			if (mResource != nullptr)
				mResource->Unmap(0, nullptr);

			mMappedResource = nullptr;
		}

		constexpr const uint32_t GetSizeInBytes() const { return m_elementSizeInBytes; }

		void CopyData(const T& data)
		{
			SetMappedData(&data, sizeof(T));
		}

	private:
		uint32_t m_elementSizeInBytes;
	};

	template<typename T>
	inline ConstantBuffer<T>::ConstantBuffer() :
		GPUBuffer(*Graphics::GetDevice(), BufferCreationDesc::Create(AlignU32(sizeof(T), 256), BufferViewFlags::cbv, BufferAccessFlags::hostWritable))
	{
		m_elementSizeInBytes = AlignU32(sizeof(T), 256);
	}
}

