#pragma once

#include "TrioAPI.h"
#include "StatesEnums.h"

namespace Cuado
{
	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL RasterizerState
	{
	public:
		RasterizerState();
		~RasterizerState();

		CullMode CullMode;
		float DepthBias;
		FillMode FillMode;
		bool MultiSampleAntiAlias;
		bool ScissorTestEnable;
		float SlopeScaleDepthBias;
		bool DepthClipEnable;
		float DepthBiasClamp;

		static RasterizerState* CullClockwise;
		static RasterizerState* CullCounterClockwise;
		static RasterizerState* CullNone;
		
		static void InitStates();
		
		friend class GraphicsDevice;
	private:
#ifdef TRIO_DIRECTX
		ID3D11RasterizerState * m_State;
#endif

		void ApplyState(GraphicsDevice * device);
	};
}
