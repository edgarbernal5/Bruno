#include "stdafx.h"
#include "IndirectPositionCollection.h"

#include "VertexChannel.h"

#include "MeshContent.h"
#include "GeometryContent.h"

namespace TrioEngine
{
	IndirectPositionCollection::IndirectPositionCollection(GeometryContent* geom, VertexChannel* positionIndices) :
		m_geometry(geom),
		m_positionIndices(positionIndices)
	{
	}

	IndirectPositionCollection::~IndirectPositionCollection()
	{
	}

	const IndirectPositionCollection::BaseType & IndirectPositionCollection::operator[](int nIndex) const
	{
		int* npointer = (int*)((*m_positionIndices)[0]) + nIndex;
		int nn = *npointer;

		return m_geometry->GetParent()->GetPositions()[nn];
	}

	int IndirectPositionCollection::GetCount()
	{
		return m_positionIndices->Count();
	}

	bool IndirectPositionCollection::Contains(Vector3 item)
	{
		return IndexOf(item) > -1;
	}

	int IndirectPositionCollection::IndexOf(Vector3& item)
	{
		for (int i = 0; i < GetCount(); i++)
		{
			if ((*this)[i].x == item.x && (*this)[i].y == item.y && (*this)[i].z == item.z)
				return i;
		}
		return -1;
	}
}