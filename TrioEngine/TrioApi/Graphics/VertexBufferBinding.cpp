#include "stdafx.h"
#include "VertexBufferBinding.h"

namespace TrioEngine
{

	VertexBufferBindings::VertexBufferBindings() :
		
		m_n(0),
		m_capacity(0)
	{
		m_capacity = 32;
		for (size_t i = 0; i < m_capacity; i++)
		{
			m_array[i] = VertexBufferBinding();
		}
	}

	VertexBufferBindings::~VertexBufferBindings()
	{
		for (size_t i = 0; i < m_capacity; i++)
		{
			m_array[i] = VertexBufferBinding();
		}
		m_n = 0;
		m_capacity = 0;
	}

	void VertexBufferBindings::clear()
	{
		m_n = 0;
	}

	VertexBufferBindings::BaseType & VertexBufferBindings::operator[](int iIndex)
	{
		if (iIndex >= m_n) {
			m_n = iIndex + 1;
		}
		return m_array[iIndex];
	}

	const VertexBufferBindings::BaseType & VertexBufferBindings::operator[](int iIndex) const
	{
		return m_array[iIndex];
	}

	size_t VertexBufferBindings::size()
	{
		return m_n;
	}


	void VertexBufferBindings::push_back(VertexBufferBinding parameter)
	{
		
	}
}