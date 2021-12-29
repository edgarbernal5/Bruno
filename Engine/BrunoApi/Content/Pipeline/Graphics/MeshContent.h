#pragma once

#include "TrioApiRequisites.h"

#include "NodeContent.h"

namespace BrunoEngine
{
	class GeometryContent;

	class BRUNO_API_EXPORT MeshContent : public NodeContent
	{
	public:
		MeshContent();
		~MeshContent();
		
		inline std::vector<Vector3>& GetPositions()
		{
			return m_positions;
		}
		inline std::vector<GeometryContent*>& GetGeometry()
		{
			return m_geometryContent;
		}
	private:
		std::vector<GeometryContent*> m_geometryContent;
		std::vector<Vector3> m_positions;
	};
}