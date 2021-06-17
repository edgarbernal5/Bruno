#include "stdafx.h"
#include "SceneSubMeshContent.h"

namespace BrunoEngine
{
	SceneSubMeshContent::SceneSubMeshContent()
	{
	}

	SceneSubMeshContent::SceneSubMeshContent(VertexBufferContent* vertexBuffer, IndexCollection* indices, int vertexOffset, int numVertices, int startIndex, int primitiveCount) :
		m_vertexBuffer(vertexBuffer),
		m_vertexOffset(vertexOffset),
		m_startIndex(startIndex),
		m_primitiveCount(primitiveCount),
		m_numVertices(numVertices)
	{
		//m_indexBuffer = new IndexCollection(*indices);
		m_indexBuffer = indices;
	}

	SceneSubMeshContent::~SceneSubMeshContent()
	{

	}
}