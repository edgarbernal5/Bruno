#include "stdafx.h"
#include "VertexBuffer.h"

#include "GraphicsDevice.h"
#include "VertexDeclaration.h"

namespace TrioEngine
{
	VertexBuffer::VertexBuffer() :
		m_vertexDeclaration(nullptr),
		m_device(nullptr),
		m_vertexCount(0),
		m_usage(ResourceUsage::Default)
	{

	}

	VertexBuffer::VertexBuffer(GraphicsDevice* device, VertexDeclaration *vertexDeclaration, int vertexCount, ResourceUsage usage) :
#ifdef BRUNO_DIRECTX
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
#ifdef BRUNO_DIRECTX
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
		m_device = nullptr;
		m_vertexDeclaration = nullptr;
	}

#ifdef BRUNO_DIRECTX
	void VertexBuffer::CreateBuffer(D3D11_SUBRESOURCE_DATA* subdata)
	{
		//D3D11_BUFFER_DESC vbd = { 0 };
		D3D11_BUFFER_DESC vbd = {};

		ResourceUsage usage = ResourceUsage::Default;
		CpuAccessFlags cpuAccessFlags = CpuAccessFlags::None;
		//ResourceUsage usage = m_eUsage;

		uint64_t sizeInBytes = uint64_t(m_vertexCount) * m_vertexDeclaration->GetVertexStride();

		if (sizeInBytes > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
			throw std::exception("Buffer too large for DirectX 11");

		if (m_usage == ResourceUsage::Dynamic) {
			usage = ResourceUsage::Dynamic;
			cpuAccessFlags = CpuAccessFlags::Write;
		}
		vbd.Usage = (D3D11_USAGE)usage;
		vbd.ByteWidth = sizeInBytes;
		vbd.BindFlags = (uint32_t)BindFlags::VertexBuffer;
		vbd.CPUAccessFlags = (uint32_t)cpuAccessFlags;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		DX::ThrowIfFailed(
			m_device->GetD3DDevice()->CreateBuffer(&vbd, subdata, m_buffer.GetAddressOf())
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