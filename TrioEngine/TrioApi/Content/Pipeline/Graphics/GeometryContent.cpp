#include "stdafx.h"
#include "GeometryContent.h"

#include "VertexContent.h"

namespace TrioEngine
{
	GeometryContent::GeometryContent() : m_parent(nullptr), m_vertices(nullptr), m_materialContent(nullptr)
	{
		m_vertices = new VertexContent(this);
	}

	GeometryContent::~GeometryContent()
	{
	}
}