#include "stdafx.h"
#include "VertexBufferBinding.h"

#include "GraphicsDevice.h"

namespace Cuado
{

	VertexBufferBindings::VertexBufferBindings() :
		m_pArray(nullptr),
		m_iN(0),
		m_iCapacity(0)
	{

	}

	VertexBufferBindings::~VertexBufferBindings() {
		if (m_pArray) {
			delete m_pArray;
			m_pArray = nullptr;
		}
		m_iN = 0;
		m_iCapacity = 0;
	}

	void VertexBufferBindings::clear()
	{

	}

	VertexBufferBindings::BaseType & VertexBufferBindings::operator[](int iIndex)
	{
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