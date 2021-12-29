#pragma once

#include "Ecs/Entity.h"

#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"

namespace BrunoEngine
{
	struct MeshComponent
	{
		struct SubMesh
		{
			Entity m_materialId = INVALID_ENTITY;
			int m_vertexOffset;
			int m_startIndex;
			int m_primitiveCount;
			int m_vertexCount;
		};

		IndexBuffer m_indexBuffer;
		VertexBuffer m_vertexBuffer;

		MeshComponent() {}

		std::vector<SubMesh> m_subMeshes;
	};
}