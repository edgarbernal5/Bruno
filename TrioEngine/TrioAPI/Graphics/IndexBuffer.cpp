#include "stdafx.h"
#include "IndexBuffer.h"


namespace Cuado
{
	IndexBuffer::IndexBuffer() :
		
#ifdef TRIO_DIRECTX
		m_pBuffer(nullptr),
#elif TRIO_OPENGL
		m_pBuffer(0),
#endif
		m_pDevice(nullptr)
	{
	
	}

	IndexBuffer::IndexBuffer(GraphicsDevice *device, IndexElementSize elementSize, int indexCount, ResourceUsage usage) :
		m_pDevice(device),
		m_pBuffer(0),
		m_iIndexCount(indexCount),
		m_eElementSize(elementSize),
		m_eUsage(usage)
	{

	}

	IndexBuffer::IndexBuffer(GraphicsDevice *device, IndexElementSize elementSize, int indexCount) :
		m_pDevice(device),
		m_pBuffer(0),
		m_iIndexCount(indexCount),
		m_eElementSize(elementSize),
		m_eUsage(ResourceUsage::Immutable)
	{

	}

	IndexBuffer::~IndexBuffer()
	{
	}

#ifdef TRIO_DIRECTX
	void IndexBuffer::CreateBuffer(D3D11_SUBRESOURCE_DATA* subdata)
	{
		D3D11_BUFFER_DESC ibd;
		ResourceUsage usage = ResourceUsage::Default;
		//ResourceUsage usage = m_eUsage;

		ibd.ByteWidth = m_iIndexCount * (m_eElementSize == IndexElementSize::SixteenBits ? 2 : 4);
		ibd.BindFlags = (uint32_t)BindFlags::IndexBuffer;
		ibd.CPUAccessFlags = 0;

		if (m_eUsage == ResourceUsage::Dynamic)
		{
			ibd.CPUAccessFlags = (uint32_t)CpuAccessFlags::Write;
			usage = ResourceUsage::Dynamic;
		}

		ibd.Usage = (D3D11_USAGE)usage;

		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		DX::ThrowIfFailed(
			m_pDevice->GetD3DDevice()->CreateBuffer(&ibd, subdata, &m_pBuffer)
		);
	}
#endif
}