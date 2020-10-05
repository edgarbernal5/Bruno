#pragma once

#include "TrioApiRequisites.h"


namespace TrioEngine
{
	class GeometryContent;
	class VertexChannel;

	class TRIO_API_EXPORT IndirectPositionCollection
	{
	public:
		typedef Vector3 BaseType;

		IndirectPositionCollection(GeometryContent* geometryContent, VertexChannel* positionIndices);
		~IndirectPositionCollection();

		const BaseType & operator[](int nIndex) const;

		int GetCount();
		bool Contains(Vector3 item);
		int IndexOf(Vector3& item);
	private:
		GeometryContent* m_geometry;
		VertexChannel* m_positionIndices;
	};
}