#include "stdafx.h"
#include "SamplerState.h"

#include "GraphicsDevice.h"

namespace Vago
{
	SamplerState* SamplerState::AnisotropicClamp = nullptr;
	SamplerState* SamplerState::AnisotropicWrap = nullptr;

	SamplerState* SamplerState::LinearClamp = nullptr;
	SamplerState* SamplerState::LinearWrap = nullptr;

	SamplerState* SamplerState::PointClamp = nullptr;
	SamplerState* SamplerState::PointWrap = nullptr;

	SamplerState::SamplerState()

#ifdef TRIO_DIRECTX
		:
	m_pState(nullptr)
#endif
	{
		Filter = TextureFilter::Linear;
		AddressU = TextureAddressMode::Wrap;
		AddressV = TextureAddressMode::Wrap;
		AddressW = TextureAddressMode::Wrap;
		MaxAnisotropy = 4;
		MaxMipLevel = 0;
		MipMapLevelOfDetailBias = 0.0f;
		MinLOD = 0.0f;
		MaxLOD = D3D11_FLOAT32_MAX;

		BorderColor.x = BorderColor.y = BorderColor.z = BorderColor.w = 0.0f;
	}

	SamplerState::~SamplerState()
	{
	}

#ifdef TRIO_DIRECTX
	ID3D11SamplerState* SamplerState::GetState(GraphicsDevice* device)
	{
		if (m_pState == nullptr)
		{
			D3D11_SAMPLER_DESC desc;

			desc.AddressU = GetAddressMode(AddressU);
			desc.AddressV = GetAddressMode(AddressV);
			desc.AddressW = GetAddressMode(AddressW);
			desc.Filter = GetFilter(Filter);

			desc.MaxAnisotropy = MaxAnisotropy;
			desc.MipLODBias = MipMapLevelOfDetailBias;

			desc.MinLOD = MinLOD;
			desc.MaxLOD = MaxLOD;

			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			memcpy(desc.BorderColor, &BorderColor.x, 4 * sizeof(float));

			DX::ThrowIfFailed(device->GetD3DDevice()->CreateSamplerState(&desc, &m_pState));
		}

		return m_pState;
	}

	D3D11_TEXTURE_ADDRESS_MODE SamplerState::GetAddressMode(TextureAddressMode mode)
	{
		switch (mode)
		{
		case TextureAddressMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;

		case TextureAddressMode::Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;

		case TextureAddressMode::Wrap:
			return D3D11_TEXTURE_ADDRESS_WRAP;

		case TextureAddressMode::Border:
			return D3D11_TEXTURE_ADDRESS_BORDER;

		default:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}
	}

	D3D11_FILTER SamplerState::GetFilter(TextureFilter filter)
	{
		//TO-DO: revisar esto:
		switch (filter)
		{
		case TextureFilter::Anisotropic:
			return D3D11_FILTER_ANISOTROPIC;

		case TextureFilter::Linear:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		case TextureFilter::LinearMipPoint:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;

		case TextureFilter::MinLinearMagPointMipLinear:
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

		case TextureFilter::MinLinearMagPointMipPoint:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;

		case TextureFilter::MinPointMagLinearMipLinear:
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;

		case TextureFilter::MinPointMagLinearMipPoint:
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;

		case TextureFilter::Point:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;

		case TextureFilter::PointMipLinear:
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;

		default:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
		}
#endif

	void SamplerState::InitStates()
	{
		if (AnisotropicClamp != nullptr)
			return;

		AnisotropicClamp = new SamplerState();
		AnisotropicClamp->Filter = TextureFilter::Anisotropic;
		AnisotropicClamp->AddressU = TextureAddressMode::Clamp;
		AnisotropicClamp->AddressV = TextureAddressMode::Clamp;
		AnisotropicClamp->AddressW = TextureAddressMode::Clamp;

		AnisotropicWrap = new SamplerState();
		AnisotropicWrap->Filter = TextureFilter::Anisotropic;
		AnisotropicWrap->AddressU = TextureAddressMode::Wrap;
		AnisotropicWrap->AddressV = TextureAddressMode::Wrap;
		AnisotropicWrap->AddressW = TextureAddressMode::Wrap;

		LinearClamp = new SamplerState();
		LinearClamp->Filter = TextureFilter::Linear;
		LinearClamp->AddressU = TextureAddressMode::Clamp;
		LinearClamp->AddressV = TextureAddressMode::Clamp;
		LinearClamp->AddressW = TextureAddressMode::Clamp;


		LinearWrap = new SamplerState();
		LinearWrap->Filter = TextureFilter::Linear;
		LinearWrap->AddressU = TextureAddressMode::Wrap;
		LinearWrap->AddressV = TextureAddressMode::Wrap;
		LinearWrap->AddressW = TextureAddressMode::Wrap;


		PointClamp = new SamplerState();
		PointClamp->Filter = TextureFilter::Point;
		PointClamp->AddressU = TextureAddressMode::Clamp;
		PointClamp->AddressV = TextureAddressMode::Clamp;
		PointClamp->AddressW = TextureAddressMode::Clamp;


		PointWrap = new SamplerState();
		PointWrap->Filter = TextureFilter::Point;
		PointWrap->AddressU = TextureAddressMode::Wrap;
		PointWrap->AddressV = TextureAddressMode::Wrap;
		PointWrap->AddressW = TextureAddressMode::Wrap;

	}
}