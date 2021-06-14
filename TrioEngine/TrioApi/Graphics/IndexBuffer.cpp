#include "stdafx.h"
#include "IndexBuffer.h"

#include "GraphicsDevice.h"

namespace TrioEngine
{
	IndexBuffer::IndexBuffer() :
		
#ifdef TRIO_DIRECTX
		m_buffer(nullptr),
#elif TRIO_OPENGL
		m_pBuffer(0),
#endif
		m_device(nullptr)
	{
	
	}

	IndexBuffer::IndexBuffer(GraphicsDevice *device, IndexElementSize elementSize, int indexCount, ResourceUsage usage) :
		m_device(device),
		m_buffer(0),
		m_indexCount(indexCount),
		m_elementSize(elementSize),
		m_usage(usage)
	{

	}

	IndexBuffer::IndexBuffer(GraphicsDevice *device, IndexElementSize elementSize, int indexCount) :
		m_device(device),
		m_buffer(0),
		m_indexCount(indexCount),
		m_elementSize(elementSize),
		m_usage(ResourceUsage::Immutable)
	{

	}

	IndexBuffer::~IndexBuffer()
	{
	}

#ifdef TRIO_DIRECTX
	void IndexBuffer::CreateBuffer(D3D11_SUBRESOURCE_DATA* subdata)
	{
		D3D11_BUFFER_DESC ibd = {};
		ResourceUsage usage = ResourceUsage::Default;
		//ResourceUsage usage = m_eUsage;

		uint64_t sizeInBytes = uint64_t(m_indexCount) * (m_elementSize == IndexElementSize::SixteenBits ? 2u : 4u);

		if (sizeInBytes > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
			throw std::exception("Buffer too large for DirectX 11");

		ibd.ByteWidth = static_cast<UINT>(sizeInBytes);
		ibd.BindFlags = (uint32_t)BindFlags::IndexBuffer;
		ibd.CPUAccessFlags = 0;

		if (m_usage == ResourceUsage::Dynamic)
		{
			ibd.CPUAccessFlags = (uint32_t)CpuAccessFlags::Write;
			usage = ResourceUsage::Dynamic;
		}

		ibd.Usage = (D3D11_USAGE)usage;

		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		DX::ThrowIfFailed(
			m_device->GetD3DDevice()->CreateBuffer(&ibd, subdata, &m_buffer)
		);
	}
#endif

	////////////////////////////////////////////////////////////////
	DynamicIndexBuffer::DynamicIndexBuffer(GraphicsDevice* device, IndexElementSize elementSize, int indexCount) :
		IndexBuffer(device, elementSize, indexCount, ResourceUsage::Dynamic),
		m_userOffset(0)
	{

	}

	DynamicIndexBuffer::~DynamicIndexBuffer()
	{
	}
}