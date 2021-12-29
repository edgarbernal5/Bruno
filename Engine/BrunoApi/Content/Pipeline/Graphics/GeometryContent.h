#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace BrunoEngine
{
	class MaterialContent;
	class MeshContent;
	class VertexContent;

	class BRUNO_API_EXPORT GeometryContent : public ContentItem
	{
	public:
		GeometryContent();
		~GeometryContent();

		inline void SetMaterial(MaterialContent* material)
		{
			m_materialContent = material;
		}

		inline VertexContent* GetVertices()
		{
			return m_vertices;
		}

		inline MeshContent* GetParent()
		{
			return m_parent;
		}

		inline void SetParent(MeshContent* parent)
		{
			m_parent = parent;
		}
		
		inline MaterialContent* GetMaterial()
		{
			return m_materialContent;
		}
		
		inline std::vector<int>& GetIndices()
		{
			return m_indices;
		}

	private:
		MeshContent* m_parent;
		std::vector<int> m_indices;

		VertexContent* m_vertices;
		MaterialContent* m_materialContent;

		std::vector<GeometryContent*> m_geometryCollection;
	};
}