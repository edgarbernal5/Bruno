#pragma once

#include "BrunoApi.h"

#include "TextureAddressMode.h"
#include "TextureFilter.h"
#include "Math\MathVector.h"

namespace TrioEngine
{
	struct BRUNO_API_EXPORT SamplerState
	{
	public:
		SamplerState();
		~SamplerState();

		TextureAddressMode AddressU;
		TextureAddressMode AddressV;
		TextureAddressMode AddressW;

		TextureFilter Filter;

		uint32_t MaxAnisotropy;
		int MaxMipLevel;
		float MipMapLevelOfDetailBias;
		float MinLOD;
		float MaxLOD;

		Vector4 BorderColor;

		static void InitStates();
		static SamplerState* AnisotropicClamp;
		static SamplerState* AnisotropicWrap;

		static SamplerState* LinearClamp;
		static SamplerState* LinearWrap;

		static SamplerState* PointClamp;
		static SamplerState* PointWrap;

		friend class SamplerStateCollection;
	private:
#ifdef BRUNO_DIRECTX
		ID3D11SamplerState * m_state;

		ID3D11SamplerState * GetState(GraphicsDevice * device);

		D3D11_TEXTURE_ADDRESS_MODE GetAddressMode(TextureAddressMode mode);
		D3D11_FILTER GetFilter(TextureFilter filter);
#endif
	};
}
