#include "stdafx.h"
#include "SamplerStateCollection.h"

#include "SamplerState.h"
#include "GraphicsDevice.h"

namespace Vago
{
	SamplerStateCollection::SamplerStateCollection(ShaderStage stage) :
		m_eStage(stage)
	{
		CreateArray();
		Clear();
	}


	SamplerStateCollection::~SamplerStateCollection()
	{
	}

	void SamplerStateCollection::CreateArray()
	{
		m_N = 16;
		m_pSamplers = new SamplerState*[m_N];
		m_Dirty = INT_MAX;
	}

	void SamplerStateCollection::Clear()
	{
		for (int i = 0; i < m_N; i++)
		{
			//m_Samplers[i] = nullptr;
			m_pSamplers[i] = SamplerState::LinearWrap;
		}
		m_Dirty = INT_MAX;
	}

	void SamplerStateCollection::BindAllSamplers(GraphicsDevice* device)
	{
		if (m_Dirty == 0)
			return;

		//TO-DO: tomar en cuenta los samplers de vertex shader
		for (int i = 0; i < m_N; i++)
		{
			int mask = 1 << i;
			if ((m_Dirty & mask) == 0)
				continue;

			SamplerState* sampler = m_pSamplers[i];
#ifdef TRIO_DIRECTX
			ID3D11SamplerState *state = nullptr;
			if (sampler != nullptr)
				state = sampler->GetState(device);

			switch (m_eStage)
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

			m_Dirty &= ~mask;
			if (m_Dirty == 0)
				break;
		}

		m_Dirty = 0;
	}

	void SamplerStateCollection::SetSampler(int index, SamplerState * tex)
	{
		m_pSamplers[index] = tex;
		m_Dirty |= 1 << index;
	}
}