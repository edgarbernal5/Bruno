#include "stdafx.h"
#include "VertexChannelCollection.h"

#include "VertexContent.h"
#include "VertexChannel.h"

namespace TrioEngine
{
	VertexChannelCollection::VertexChannelCollection(VertexContent* parent) :
		m_parent(parent)
	{
	}

	VertexChannelCollection::~VertexChannelCollection()
	{
		Clear();
	}

	void VertexChannelCollection::Clear()
	{
		ClearAll();
	}

	void VertexChannelCollection::ClearAll()
	{
		for (int i = 0; i < m_channels.size(); i++)
		{
			delete m_channels[i];
		}
		m_channels.clear();
	}

	bool VertexChannelCollection::Contains(VertexChannel* item)
	{
		for (int i = 0; i < m_channels.size(); i++)
		{
			if (m_channels[i] == item)
				return true;
		}
		return false;
	}

	bool VertexChannelCollection::Contains(std::string name)
	{
		for (int i = 0; i < m_channels.size(); i++)
		{
			if (m_channels[i]->GetName() == name)
				return true;
		}
		return false;
	}

	int VertexChannelCollection::Count()
	{
		return m_channels.size();
	}
}