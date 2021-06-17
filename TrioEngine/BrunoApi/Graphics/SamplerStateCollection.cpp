#include "stdafx.h"
#include "SamplerStateCollection.h"

#include "SamplerState.h"
#include "GraphicsDevice.h"

namespace TrioEngine
{
	SamplerStateCollection::SamplerStateCollection(ShaderStage stage) :
		m_stage(stage)
	{
		CreateArray();
		Clear();
	}


	SamplerStateCollection::~SamplerStateCollection()
	{
	}

	void SamplerStateCollection::CreateArray()
	{
		m_n = 16;
		m_samplers = new SamplerState*[m_n];
		m_dirty = INT_MAX;
	}

	void SamplerStateCollection::Clear()
	{
		for (int i = 0; i < m_n; i++)
		{
			//m_Samplers[i] = nullptr;
			m_samplers[i] = SamplerState::LinearWrap;
		}
		m_dirty = INT_MAX;
	}

	void SamplerStateCollection::BindAllSamplers(GraphicsDevice* device)
	{
		if (m_dirty == 0)
			return;

		//TO-DO: tomar en cuenta los samplers de vertex shader
		for (int i = 0; i < m_n; i++)
		{
			int mask = 1 << i;
			if ((m_dirty & mask) == 0)
				continue;

			SamplerState* sampler = m_samplers[i];
#ifdef BRUNO_DIRECTX
			ID3D11SamplerState *state = nullptr;
			if (sampler != nullptr)
				state = sampler->GetState(device);

			switch (m_stage)
			{
			case ShaderStage::Vertex:
				device->GetD3DDeviceContext()->VSSetSamplers(i, 1, &state);
				break;
			case ShaderStage::Pixel:
				device->GetD3DDeviceContext()->PSSetSamplers(i, 1, &state);
				break;
			case ShaderStage::Geometry:
				break;
			}
#endif

			m_dirty &= ~mask;
			if (m_dirty == 0)
				break;
		}

		m_dirty = 0;
	}

	void SamplerStateCollection::SetSampler(int index, SamplerState * tex)
	{
		m_samplers[index] = tex;
		m_dirty |= 1 << index;
	}
}