#pragma once

#include "TrioApiDecl.h"

#include "CullMode.h"
#include "FillMode.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class TRIOAPI_DECL RasterizerState
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
		ID3D11RasterizerState * m_state;
#endif

		void ApplyState(GraphicsDevice * device);
	};
}
