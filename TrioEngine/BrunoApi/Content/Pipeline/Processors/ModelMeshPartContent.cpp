#include "stdafx.h"
#include "ModelMeshPartContent.h"

namespace TrioEngine
{
	ModelMeshPartContent::ModelMeshPartContent()
	{
	}

	ModelMeshPartContent::ModelMeshPartContent(VertexBufferContent* vertexBuffer, IndexCollection* indices, int vertexOffset, int numVertices, int startIndex, int primitiveCount) :
		m_vertexBuffer(vertexBuffer),
		m_vertexOffset(vertexOffset),
		m_startIndex(startIndex),
		m_primitiveCount(primitiveCount),
		m_numVertices(numVertices),
		m_indexBuffer(indices),
		m_material(nullptr)
	{
	}

	ModelMeshPartContent::~ModelMeshPartContent()
	{

	}
}