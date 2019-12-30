#include "stdafx.h"
#include "VertexChannel.h"

namespace TrioEngine
{
	VertexChannel::VertexChannel(std::string name) :
		m_name(name),
		m_sizeInBytesOfT(0),
		m_itemsSize(0),
		m_rawDataSize(0)
	{
	}

	VertexChannel::~VertexChannel()
	{
	}
}