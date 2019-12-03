#pragma once

#include "TrioAPI.h"

#include "ShaderStage.h"

namespace TrioEngine
{
	struct SamplerState;
	class GraphicsDevice;

	class TRIO_API_EXPORT SamplerStateCollection
	{
	public:
		SamplerStateCollection(ShaderStage stage);
		~SamplerStateCollection();

		SamplerState* operator [](const int index) const
		{
			return m_samplers[index];
		}

		SamplerState* & operator[](int nIndex)
		{
			m_dirty |= 1 << nIndex;
			return m_samplers[nIndex];
		}

		void Clear();
		void BindAllSamplers(GraphicsDevice * m_Device);
		void SetSampler(int index, SamplerState* tex);

	private:
		SamplerState** m_samplers;
		int m_dirty;
		int m_n;
		ShaderStage m_stage;

		void CreateArray();
	};
}
