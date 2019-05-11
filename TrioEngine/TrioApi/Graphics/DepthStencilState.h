#pragma once

#include "TrioApiDecl.h"

#include "CompareFunction.h"
#include "DepthWriteMask.h"
#include "StencilOperation.h"

namespace TrioEngine
{
	class GraphicsDevice;

	struct TRIOAPI_DECL DepthStencilState
	{
	public:
		DepthStencilState();
		~DepthStencilState();

		bool DepthBufferEnable;
		bool DepthBufferWriteEnable;

		StencilOperation CounterClockwiseStencilDepthBufferFail;
		StencilOperation CounterClockwiseStencilFail;

		CompareFunction CounterClockwiseStencilFunction;

		StencilOperation CounterClockwiseStencilPass;
		CompareFunction DepthBufferFunction;
		int ReferenceStencil;
		StencilOperation StencilDepthBufferFail;
		bool StencilEnable;
		StencilOperation StencilFail;
		CompareFunction StencilFunction;
		uint8_t StencilMask;
		StencilOperation StencilPass;
		uint8_t StencilWriteMask;
		bool TwoSidedStencilMode;

		
		static DepthStencilState* Default;
		static DepthStencilState* DepthRead;
		static DepthStencilState* None;
		
		static void InitStates();

#ifdef TRIO_DIRECTX
		inline static D3D11_COMPARISON_FUNC GetComparison(CompareFunction compare);
		inline static D3D11_STENCIL_OP GetStencilOp(StencilOperation op);
#endif

		friend class GraphicsDevice;
	private:
#ifdef TRIO_DIRECTX
		ID3D11DepthStencilState*		m_pState;
#endif

		void ApplyState(GraphicsDevice * device);
	};
}
