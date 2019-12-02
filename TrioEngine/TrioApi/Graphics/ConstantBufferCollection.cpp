#include "stdafx.h"
#include "ConstantBufferCollection.h"

#include "GraphicsDevice.h"

namespace TrioEngine
{
	ConstantBufferCollection::ConstantBufferCollection() : 
		m_count(0),
		m_stage(ShaderStage::Vertex)
	{
		m_valid = 0;
		for (size_t i = 0; i < 16; i++)
		{
			m_arrayBuffers[i] = nullptr;
		}
	}

	ConstantBufferCollection::ConstantBufferCollection(ShaderStage stage) : 
		m_count(0), 
		m_stage(stage)
	{
		m_valid = 0;
		for (size_t i = 0; i < 16; i++)
		{
			m_arrayBuffers[i] = nullptr;
		}
	}

	void ConstantBufferCollection::clear()
	{
		m_valid = 0;
		
		for (size_t i = 0; i < 16; i++)
		{
			m_arrayBuffers[i] = nullptr;
		}
		m_count = 0;
	}

	void ConstantBufferCollection::push_back(ConstantBuffer* item)
	{
		if (m_count < 15)
		{
			m_arrayBuffers[m_count] = item;
			m_count++;
		}

	}


#ifdef TRIO_DIRECTX
	void ConstantBufferCollection::SetConstantBuffers(GraphicsDevice* device)
	{
		if (m_valid == 0)
		{
			return;
		}

		int valid = m_valid;
		for (int i = 0; i < 16; i++)
		{
			ConstantBuffer* item = m_arrayBuffers[i];

			if (item != nullptr)
			{
				item->Apply(m_stage, i);
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
		return m_count;
	}
}
