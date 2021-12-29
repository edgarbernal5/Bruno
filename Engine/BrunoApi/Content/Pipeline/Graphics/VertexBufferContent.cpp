#include "stdafx.h"
#include "VertexBufferContent.h"

#include "VertexDeclarationContent.h"

namespace BrunoEngine
{
	VertexBufferContent::VertexBufferContent()
	{
		m_vertexSize = 0;
		m_vertexData = new uint8_t[m_vertexSize];
		m_vertexDeclaration = new VertexDeclarationContent();

		m_processorName = "VertexBufferWriter";
	}

	VertexBufferContent::VertexBufferContent(uint32_t size)
	{
		m_vertexSize = size;
		m_vertexData = new uint8_t[size];
		m_vertexDeclaration = new VertexDeclarationContent();

		m_processorName = "VertexBufferWriter";
	}

	VertexBufferContent::~VertexBufferContent()
	{
	}
}