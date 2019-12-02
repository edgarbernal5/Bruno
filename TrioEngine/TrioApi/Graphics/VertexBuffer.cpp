#include "stdafx.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"
#include "VertexDeclaration.h"

namespace TrioEngine
{
	VertexBuffer::VertexBuffer() :
		
#ifdef TRIO_DIRECTX
		m_buffer(nullptr),
#elif TRIO_OPENGL
		m_pBuffer(0),
#endif
		m_device(nullptr) {
	
	}

	VertexBuffer::VertexBuffer(GraphicsDevice* device, VertexDeclaration *vertexDeclaration, int vertexCount, ResourceUsage usage) :
#ifdef TRIO_DIRECTX
		m_buffer(nullptr),
#elif OPENGL
		m_pBuffer(0),
#endif
		m_vertexDeclaration(vertexDeclaration),
		m_vertexCount(vertexCount),
		m_usage(usage),

		m_device(device)
	{

	}

	VertexBuffer::VertexBuffer(GraphicsDevice* device, VertexDeclaration *vertexDeclaration, int vertexCount) :
#ifdef TRIO_DIRECTX
		m_buffer(nullptr),
#elif OPENGL
		m_pBuffer(0),
#endif
		m_vertexDeclaration(vertexDeclaration),
		m_vertexCount(vertexCount),
		m_usage(ResourceUsage::Immutable),

		m_device(device)
	{

	}

	VertexBuffer::~VertexBuffer()
	{
	}

#ifdef TRIO_DIRECTX
	void VertexBuffer::CreateBuffer(D3D11_SUBRESOURCE_DATA* subdata)
	{
		D3D11_BUFFER_DESC vbd = {};

		ResourceUsage usage = ResourceUsage::Default;
		//ResourceUsage usage = m_eUsage;

		uint64_t sizeInBytes = uint64_t(m_vertexCount) * m_vertexDeclaration->GetVertexStride();

		if (sizeInBytes > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
			throw std::exception("Buffer too large for DirectX 11");

		vbd.Usage = (D3D11_USAGE)usage;
		vbd.ByteWidth = sizeInBytes;
		vbd.BindFlags = (uint32_t)BindFlags::VertexBuffer;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		DX::ThrowIfFailed(
			m_device->GetD3DDevice()->CreateBuffer(&vbd, subdata, &m_buffer)
		);
	}
#endif


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	DynamicVertexBuffer::DynamicVertexBuffer(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount) :
		VertexBuffer(device, vertexDeclaration, vertexCount, ResourceUsage::Dynamic),
		m_userOffset(0)
	{

	}

	DynamicVertexBuffer::~DynamicVertexBuffer()
	{
	}
}