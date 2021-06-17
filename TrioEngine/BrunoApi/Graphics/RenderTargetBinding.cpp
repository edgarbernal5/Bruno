#include "stdafx.h"
#include "RenderTargetBinding.h"

namespace TrioEngine
{
	RenderTargetBindings::RenderTargetBindings() :
		m_n(0),
		m_capacity(0)
	{
		m_capacity = 32;
		for (size_t i = 0; i < m_capacity; i++)
		{
			m_array[i] = RenderTargetBinding();
		}
	}

	RenderTargetBindings::RenderTargetBindings(RenderTargetBinding binding) :
		m_n(1),
		m_capacity(0)
	{
		m_capacity = 32;
		m_array[0] = binding;

		for (size_t i = 1; i < m_capacity; i++)
		{
			m_array[i] = RenderTargetBinding();
		}
	}

	RenderTargetBindings::~RenderTargetBindings()
	{
		for (size_t i = 0; i < m_capacity; i++)
		{
			m_array[i] = RenderTargetBinding();
		}
		m_n = 0;
		m_capacity = 0;
	}

	void RenderTargetBindings::clear()
	{
		m_n = 0;
	}

	RenderTargetBindings::BaseType & RenderTargetBindings::operator[](int iIndex)
	{
		if (iIndex >= m_n) {
			m_n = iIndex + 1;
		}
		return m_array[iIndex];
	}

	const RenderTargetBindings::BaseType & RenderTargetBindings::operator[](int iIndex) const
	{
		return m_array[iIndex];
	}

	size_t RenderTargetBindings::size()
	{
		return m_n;
	}


	void RenderTargetBindings::push_back(RenderTargetBinding& parameter)
	{
		if (m_capacity >= m_n) return;

		m_array[m_n++] = parameter;
	}
}