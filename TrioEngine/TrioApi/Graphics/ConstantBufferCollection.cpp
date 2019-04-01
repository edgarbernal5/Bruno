#include "stdafx.h"
#include "ConstantBufferCollection.h"

#include "GraphicsDevice.h"

namespace Vago
{
	ConstantBufferCollection::ConstantBufferCollection() : m_iCount(0)
	{
		m_iValid = 0;
		for (size_t i = 0; i < 16; i++)
		{
			m_pArrayBuffers[i] = nullptr;
		}
	}

	ConstantBufferCollection::ConstantBufferCollection(ShaderStage stage) : 
		m_iCount(0), 
		m_eStage(stage)
	{
		m_iValid = 0;
		for (size_t i = 0; i < 16; i++)
		{
			m_pArrayBuffers[i] = nullptr;
		}
	}

	void ConstantBufferCollection::clear()
	{
		m_iValid = 0;
		
		for (size_t i = 0; i < 16; i++)
		{
			m_pArrayBuffers[i] = nullptr;
		}
		m_iCount = 0;
	}

	void ConstantBufferCollection::push_back(ConstantBuffer* item)
	{
		if (m_iCount < 15)
		{
			m_pArrayBuffers[m_iCount] = item;
			m_iCount++;
		}

	}


#ifdef TRIO_DIRECTX
	void ConstantBufferCollection::SetConstantBuffers(GraphicsDevice* device)
	{
		if (m_iValid == 0)
		{
			return;
		}

		int valid = m_iValid;
		for (int i = 0; i < 16; i++)
		{
			ConstantBuffer* item = m_pArrayBuffers[i];

			if (item != nullptr)
			{
				item->Apply(m_eStage, i);
			}

			valid &= ~(1 << i);
			if (valid == 0)
			{
				return;
			}
		}
	}
#endif

#if TRIO_OPENGL
	void ConstantBufferCollection::SetConstantBuffers(GraphicsDevice* device, ShaderProgram* program)
	{
		if (m_iValid == 0)
		{
			return;
		}

		int valid = m_iValid;
		for (int i = 0; i < 16; i++)
		{
			ConstantBuffer* item = m_pArrayBuffers[i];

			if (item != nullptr)
			{
				item->Apply(program);
			}

			valid &= ~(1 << i);
			if (valid == 0)
			{
				return;
			}
		}
	}
#endif

	size_t ConstantBufferCollection::size()
	{
		return m_iCount;
	}
}
