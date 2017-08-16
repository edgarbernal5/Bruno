#include "stdafx.h"
#include "VertexBuffer.h"

#include "VertexDeclaration.h"

namespace Cuado
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
		D3D11_BUFFER_DESC vbd;

		vbd.Usage = (D3D11_USAGE)m_eUsage;
		vbd.ByteWidth = m_iVertexCount * m_pVertexDeclaration->GetVertexStride();
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