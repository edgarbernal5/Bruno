#pragma once



#include "TrioAPI.h"

#include "StatesEnums.h"

namespace Cuado
{
	class TRIOAPI_DLL SamplerState;
	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL SamplerStateCollection
	{
	public:
		SamplerStateCollection(ShaderStage stage);
		~SamplerStateCollection();
		
		SamplerState* operator [](const int index) const
		{
			return m_Samplers[index];
		}
		

		SamplerState* & operator[](int nIndex)
		{
			m_Dirty |= 1 << nIndex;
			return m_Samplers[nIndex];
		}

		void Clear();
		void SetSampler(int index, SamplerState* tex);
		void BindAllSamplers(GraphicsDevice * m_Device);

	private:
		SamplerState** m_Samplers;
		int m_Dirty;
		int m_N;
		ShaderStage m_Stage;
		//GraphicsDevice * m_Device;

		void CreateArray();
	};
}
