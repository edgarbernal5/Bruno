#include "stdafx.h"
#include "ModelMeshPart.h"


namespace TrioEngine
{
	ModelMeshPart::ModelMeshPart() : m_diffuseTexture(nullptr), 
		m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_vertexOffset(0), m_startIndex(0),
		m_material(nullptr)
	{
	}

	ModelMeshPart::ModelMeshPart(uint32_t vertexOffset, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount) :
		m_diffuseTexture(nullptr), m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_vertexOffset(0), m_startIndex(0)
	{
		m_vertexOffset = vertexOffset;
		m_vertexCount = numVertices;
		m_startIndex = startIndex;
		m_primitiveCount = primitiveCount;
	}

	ModelMeshPart::~ModelMeshPart()
	{
	}
}