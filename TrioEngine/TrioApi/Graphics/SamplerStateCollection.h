#pragma once

#include "TrioAPI.h"

#include "ShaderStage.h"

namespace Vago
{
	struct SamplerState;
	class GraphicsDevice;

	class TRIOAPI_DECL SamplerStateCollection
	{
	public:
		SamplerStateCollection(ShaderStage stage);
		~SamplerStateCollection();

		SamplerState* operator [](const int index) const
		{
			return m_pSamplers[index];
		}

		SamplerState* & operator[](int nIndex)
		{
			m_Dirty |= 1 << nIndex;
			return m_pSamplers[nIndex];
		}

		void Clear();
		void BindAllSamplers(GraphicsDevice * m_Device);
		void SetSampler(int index, SamplerState* tex);

	private:
		SamplerState** m_pSamplers;
		int m_Dirty;
		int m_N;
		ShaderStage m_eStage;
		//GraphicsDevice * m_Device;

		void CreateArray();
	};
}
