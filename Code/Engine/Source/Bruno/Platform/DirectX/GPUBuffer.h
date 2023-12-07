#pragma once

#include "Resource.h"
#include "Resources.h"
#include "Bruno/Core/Base.h"
#include "Bruno/Core/Memory.h"

namespace Bruno
{
	class GrapicsDevice;
	class GpuBuffer;

	enum class BufferAccessFlags : uint8_t
	{
		GpuOnly = 0,
		HostWritable = 1
	};
	BR_DEFINITION_FLAG_FROM_ENUM(BufferAccessFlags);

	enum class BufferViewFlags : uint8_t
	{
		None = 0,
		Cbv = 1,
		Srv = 2,
		Uav = 4
	};
	BR_DEFINITION_FLAG_FROM_ENUM(BufferViewFlags);

	struct BufferCreationDesc
	{
		uint32_t Size = 0;
		uint32_t Stride = 0;
		BufferViewFlags ViewFlags = BufferViewFlags::None;
		BufferAccessFlags AccessFlags = BufferAccessFlags::GpuOnly;
		bool IsRawAccess = false;

		static BufferCreationDesc Create(uint32_t sizeInBytes, uint32_t stride, BufferViewFlags viewFlags = BufferViewFlags::None, BufferAccessFlags accessFlags = BufferAccessFlags::GpuOnly)
		{
			BufferCreationDesc creationDesc{};

			creationDesc.Size = sizeInBytes;
			creationDesc.AccessFlags = accessFlags;
			creationDesc.ViewFlags = viewFlags;
			creationDesc.Stride = stride;
			creationDesc.IsRawAccess = true;

			return creationDesc;
		}

		static BufferCreationDesc Create(uint32_t sizeInBytes, BufferViewFlags viewFlags = BufferViewFlags::None, BufferAccessFlags accessFlags = BufferAccessFlags::GpuOnly)
		{
			BufferCreationDesc creationDesc{};

			creationDesc.Size = sizeInBytes;
			creationDesc.AccessFlags = accessFlags;
			creationDesc.ViewFlags = viewFlags;

			return creationDesc;
		}
	};

	struct BufferUpload
	{
		GpuBuffer* Buffer = nullptr;
		std::unique_ptr<uint8_t[]> BufferData;
		size_t BufferDataSize = 0;
	};

	class GpuBuffer : public Resource
	{
		BR_RTTI_DECLARATION(GpuBuffer, Resource);

	public:
		GpuBuffer(GraphicsDevice& device, const BufferCreationDesc& desc);
		virtual ~GpuBuffer();

		uint32_t GetElementCount() { return m_numElements; }
		uint8_t* GetMappedResource() { return m_mappedResource; }

		void Reset();
		virtual void SetMappedData(const void* data, size_t dataSize);

		friend class UploadContext;
		friend class GraphicsContext;
		friend class GraphicsDevice;

	protected:
		uint8_t* m_mappedResource{ nullptr };
		uint32_t m_stride = 0; 
		uint32_t m_numElements = 0;
		bool m_isHostVisible{ false };
		DescriptorHandle m_cbvDescriptor{};
		DescriptorHandle m_srvDescriptor{};
		DescriptorHandle m_uavDescriptor{};
	};
}

