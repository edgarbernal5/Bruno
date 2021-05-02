#pragma once

#include "TrioApiRequisites.h"

#include "Math/MathVector.h"

#include <string>
#include <vector>

namespace TrioEngine
{
	class VertexBufferContent;
	class IndexCollection;
	class MaterialContent;

	class TRIO_API_EXPORT SceneSubMeshContent
	{
	public:
		SceneSubMeshContent();
		SceneSubMeshContent(VertexBufferContent* vertexBuffer, IndexCollection* indices, int vertexOffset, int numVertices, int startIndex, int primitiveCount);
		~SceneSubMeshContent();

		inline int GetVertexOffset()
		{
			return m_vertexOffset;
		}

		inline int GetNumVertices()
		{
			return m_numVertices;
		}

		inline int GetPrimitiveCount()
		{
			return m_primitiveCount;
		}

		inline int GetStartIndex()
		{
			return m_startIndex;
		}
		inline void SetMaterial(MaterialContent* material)
		{
			m_material = material;
		}

		inline MaterialContent* GetMaterial()
		{
			return m_material;
		}

		inline VertexBufferContent* GetVertexBuffer()
		{
			return m_vertexBuffer;
		}

		inline IndexCollection* GetIndexBuffer()
		{
			return m_indexBuffer;
		}
	private:
		IndexCollection* m_indexBuffer;
		MaterialContent* m_material;

		int m_numVertices;
		int m_primitiveCount;
		int m_startIndex;

		VertexBufferContent* m_vertexBuffer;
		int m_vertexOffset;
	};
}