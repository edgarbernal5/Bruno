#include "stdafx.h"
#include "ModelMeshPart.h"

namespace BrunoEngine
{
	ModelMeshPart::ModelMeshPart() : m_diffuseTexture(nullptr), 
		m_vertexBuffer(nullptr), m_indexBuffer(nullptr), 
		m_vertexOffset(0), m_startIndex(0), m_vertexCount(0), m_primitiveCount(0),
		m_material(nullptr)
	{
	}

	ModelMeshPart::ModelMeshPart(uint32_t vertexOffset, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount) :
		m_diffuseTexture(nullptr), m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_vertexOffset(0), m_startIndex(0),
		m_primitiveType(PrimitiveType::TriangleList),
		m_material(nullptr)
	{
		m_vertexOffset = vertexOffset;
		m_vertexCount = numVertices;
		m_startIndex = startIndex;
		m_primitiveCount = primitiveCount;
	}

	ModelMeshPart::ModelMeshPart(uint32_t vertexOffset, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, Material* material) :
		m_diffuseTexture(nullptr), m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer), m_vertexOffset(0), m_startIndex(0),
		m_primitiveType(PrimitiveType::TriangleList),
		m_material(material)
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