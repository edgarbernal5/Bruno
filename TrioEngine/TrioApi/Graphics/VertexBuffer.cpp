#include "stdafx.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"
#include "VertexDeclaration.h"

namespace TrioEngine
{
	VertexBuffer::VertexBuffer() :
		
#ifdef TRIO_DIRECTX
		m_pBuffer(nullptr),
#elif TRIO_OPENGL
		m_pBuffer(0),
#endif
		m_pDevice(nullptr) {
	
	}

	VertexBuffer::VertexBuffer(GraphicsDevice* device, VertexDeclaration *vertexDeclaration, int vertexCount, ResourceUsage usage) :
#ifdef TRIO_DIRECTX
		m_pBuffer(nullptr),
#elif OPENGL
		m_pBuffer(0),
#endif
		m_pVertexDeclaration(vertexDeclaration),
		m_iVertexCount(vertexCount),
		m_eUsage(usage),

		m_pDevice(device)
	{

	}

	VertexBuffer::VertexBuffer(GraphicsDevice* device, VertexDeclaration *vertexDeclaration, int vertexCount) :
#ifdef TRIO_DIRECTX
		m_pBuffer(nullptr),
#elif OPENGL
		m_pBuffer(0),
#endif
		m_pVertexDeclaration(vertexDeclaration),
		m_iVertexCount(vertexCount),
		m_eUsage(ResourceUsage::Immutable),

		m_pDevice(device)
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

		uint64_t sizeInBytes = uint64_t(m_iVertexCount) * m_pVertexDeclaration->GetVertexStride();

		if (sizeInBytes > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
			throw std::exception("Buffer too large for DirectX 11");

		vbd.Usage = (D3D11_USAGE)usage;
		vbd.ByteWidth = sizeInBytes;
		vbd.BindFlags = (uint32_t)BindFlags::VertexBuffer;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		DX::ThrowIfFailed(
			m_pDevice->GetD3DDevice()->CreateBuffer(&vbd, subdata, &m_pBuffer)
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