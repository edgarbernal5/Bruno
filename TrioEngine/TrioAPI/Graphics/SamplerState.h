#pragma once

#include "TrioAPI.h"
//#include "SimpleMath.h"

namespace Cuado
{
	enum class TextureAddressMode
	{
		Wrap,
		Clamp,
		Mirror,
		Border
	};
	
	enum class TextureFilter
	{
		Linear,			// 	Use linear filtering.
		Point,			// 	Use point filtering.
		Anisotropic,	// 	Use anisotropic filtering.
		LinearMipPoint,	// 	Use linear filtering to shrink or expand, and point filtering between mipmap levels (mip).
		PointMipLinear,	// 	Use point filtering to shrink (minify) or expand (magnify), and linear filtering between mipmap levels.
		MinLinearMagPointMipLinear,	// 	Use linear filtering to shrink, point filtering to expand, and linear filtering between mipmap levels.
		MinLinearMagPointMipPoint,	// 	Use linear filtering to shrink, point filtering to expand, and point filtering between mipmap levels.
		MinPointMagLinearMipLinear,	// 	Use point filtering to shrink, linear filtering to expand, and linear filtering between mipmap levels.
		MinPointMagLinearMipPoint,	// 	Use point filtering to shrink, linear filtering to expand, and point filtering between mipmap levels.
	};

	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL SamplerState
	{
	public:
		SamplerState();
		~SamplerState();

		TextureAddressMode AddressU;
		TextureAddressMode AddressV;
		TextureAddressMode AddressW;
		
		TextureFilter Filter;
		
		UINT MaxAnisotropy;
		int MaxMipLevel;
		float MipMapLevelOfDetailBias;
		float MinLOD;
		float MaxLOD;

		Vector4 BorderColor;
		
		static void InitStates();
		static SamplerState * AnisotropicClamp;
		static SamplerState * AnisotropicWrap;

		static SamplerState * LinearClamp;
		static SamplerState * LinearWrap;
		
		static SamplerState * PointClamp;
		static SamplerState * PointWrap;

		friend class SamplerStateCollection;
	private:
#ifdef TRIO_DIRECTX
		ID3D11SamplerState * m_State;

		ID3D11SamplerState * GetState(GraphicsDevice * device);
		
		D3D11_TEXTURE_ADDRESS_MODE GetAddressMode(TextureAddressMode mode);
		D3D11_FILTER GetFilter(TextureFilter filter);
#endif
	};
}

