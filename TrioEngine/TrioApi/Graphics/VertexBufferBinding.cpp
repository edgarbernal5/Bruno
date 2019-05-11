#include "stdafx.h"
#include "VertexBufferBinding.h"

namespace TrioEngine
{

	VertexBufferBindings::VertexBufferBindings() :
		
		m_iN(0),
		m_iCapacity(0)
	{
		m_iCapacity = 32;
		for (size_t i = 0; i < m_iCapacity; i++)
		{
			m_pArray[i] = VertexBufferBinding();
		}
	}

	VertexBufferBindings::~VertexBufferBindings()
	{
		for (size_t i = 0; i < m_iCapacity; i++)
		{
			m_pArray[i] = VertexBufferBinding();
		}
		m_iN = 0;
		m_iCapacity = 0;
	}

	void VertexBufferBindings::clear()
	{
		m_iN = 0;
	}

	VertexBufferBindings::BaseType & VertexBufferBindings::operator[](int iIndex)
	{
		if (iIndex >= m_iN) {
			m_iN = iIndex + 1;
		}
		return m_pArray[iIndex];
	}

	const VertexBufferBindings::BaseType & VertexBufferBindings::operator[](int iIndex) const
	{
		return m_pArray[iIndex];
	}

	size_t VertexBufferBindings::size()
	{
		return m_iN;
	}


	void VertexBufferBindings::push_back(VertexBufferBinding parameter)
	{
		
	}
}