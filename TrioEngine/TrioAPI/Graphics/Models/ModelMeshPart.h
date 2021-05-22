#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioEngine
{
	class VertexBuffer;
	class IndexBuffer;
	class ModelBone;
	class ModelMesh;
	class Material;
	class Texture2D;

	class TRIO_API_EXPORT ModelMeshPart
	{
	public:
		ModelMeshPart();
		ModelMeshPart(uint32_t vertexOffset, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount);
		ModelMeshPart(uint32_t vertexOffset, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, Material* material);
		~ModelMeshPart();

		inline Texture2D* GetDiffuseTexture()
		{
			return m_diffuseTexture;
		}

		inline VertexBuffer* GetVertexBuffer()
		{
			return m_vertexBuffer;
		}

		inline IndexBuffer* GetIndexBuffer()
		{
			return m_indexBuffer;
		}

		inline Material* GetMaterial()
		{
			return m_material;
		}

		inline uint32_t GetVertexCount()
		{
			return m_vertexCount;
		}

		inline uint32_t GetPrimitiveCount()
		{
			return m_primitiveCount;
		}

		inline uint32_t GetVertexOffset()
		{
			return m_vertexOffset;
		}

		inline uint32_t GetStartIndex()
		{
			return m_startIndex;
		}

		inline PrimitiveType GetPrimitiveType()
		{
			return m_primitiveType;
		}

		friend class Model;

		friend class ModelReader;

	private:

		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;

		uint32_t m_vertexCount;
		uint32_t m_primitiveCount;
		uint32_t m_vertexOffset;
		uint32_t m_startIndex;

		PrimitiveType m_primitiveType;

		Texture2D* m_diffuseTexture;
		Material* m_material;
	};
}