#pragma once

#include "TrioAPI.h"

#include "StatesEnums.h"


namespace Cuado
{
	class TRIOAPI_DLL BlendState
	{
	public:
		BlendState();
		~BlendState();

		BlendFunction AlphaBlendFunction;
		Blend AlphaDestinationBlend;
		Blend AlphaSourceBlend;
		DirectX::SimpleMath::Vector4 BlendFactor;
		BlendFunction ColorBlendFunction;
		Blend ColorDestinationBlend;
		Blend ColorSourceBlend;
		ColorWriteChannels ColorWriteChannels0;
		ColorWriteChannels ColorWriteChannels1;
		ColorWriteChannels ColorWriteChannels2;
		ColorWriteChannels ColorWriteChannels3;
		int MultiSampleMask;

		static BlendState* Additive;
		static BlendState* AlphaBlend;
		static BlendState* NonPremultiplied;
		static BlendState* Opaque;

		static void InitStates();

#ifdef TRIO_DIRECTX
		inline static D3D11_BLEND_OP GetBlendOperation(BlendFunction blend);
		static D3D11_BLEND GetBlendOption(Blend blend, bool alpha);
#endif
		uint8_t GetColorWriteMask(ColorWriteChannels mask);

		friend class GraphicsDevice;
	private:
#ifdef TRIO_DIRECTX
		ID3D11BlendState*		m_pState;
#endif

		void ApplyState(GraphicsDevice* device);
	};
}

