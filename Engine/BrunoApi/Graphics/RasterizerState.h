#pragma once

#include "TrioApiRequisites.h"

#include "CullMode.h"
#include "FillMode.h"

namespace BrunoEngine
{
	class GraphicsDevice;

	class BRUNO_API_EXPORT RasterizerState
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
#ifdef BRUNO_DIRECTX
		ID3D11RasterizerState * m_state;
#endif

		void ApplyState(GraphicsDevice * device);
	};
}
