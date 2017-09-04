#include "stdafx.h"
#include "BlendState.h"


#include "GraphicsDevice.h"
namespace Cuado
{
	BlendState* BlendState::Additive = nullptr;
	BlendState* BlendState::AlphaBlend = nullptr;
	BlendState* BlendState::NonPremultiplied = nullptr;
	BlendState* BlendState::Opaque = nullptr;

	BlendState::BlendState()
#ifdef TRIO_DIRECTX
		:
	m_pState(nullptr)
#endif
	{
		AlphaBlendFunction = BlendFunction::Add;
		AlphaDestinationBlend = Blend::Zero;
		AlphaSourceBlend = Blend::One;
		BlendFactor = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f };
		ColorBlendFunction = BlendFunction::Add;
		ColorDestinationBlend = Blend::Zero;
		ColorSourceBlend = Blend::One;
		ColorWriteChannels0 = ColorWriteChannels::All;
		ColorWriteChannels1 = ColorWriteChannels::All;
		ColorWriteChannels2 = ColorWriteChannels::All;
		ColorWriteChannels3 = ColorWriteChannels::All;
		MultiSampleMask = INT_MAX;
	}

	BlendState::~BlendState()
	{
	}

	void BlendState::ApplyState(GraphicsDevice* device)
	{
#ifdef TRIO_DIRECTX
		if (m_pState == nullptr)
		{
			D3D11_BLEND_DESC desc = { 0 };

			D3D11_RENDER_TARGET_BLEND_DESC targetDesc;

			targetDesc.BlendEnable = !(ColorSourceBlend == Blend::One &&
				ColorDestinationBlend == Blend::Zero &&
				AlphaSourceBlend == Blend::One &&
				AlphaDestinationBlend == Blend::Zero);

			targetDesc.BlendOp = GetBlendOperation(ColorBlendFunction);
			targetDesc.SrcBlend = GetBlendOption(ColorSourceBlend, false);
			targetDesc.DestBlend = GetBlendOption(ColorDestinationBlend, false);

			targetDesc.BlendOpAlpha = GetBlendOperation(AlphaBlendFunction);
			targetDesc.SrcBlendAlpha = GetBlendOption(AlphaSourceBlend, false);
			targetDesc.DestBlendAlpha = GetBlendOption(AlphaDestinationBlend, false);

			desc.RenderTarget[0] = targetDesc;
			desc.RenderTarget[1] = targetDesc;
			desc.RenderTarget[2] = targetDesc;
			desc.RenderTarget[3] = targetDesc;

			desc.RenderTarget[0].RenderTargetWriteMask = GetColorWriteMask(ColorWriteChannels0);
			desc.RenderTarget[1].RenderTargetWriteMask = GetColorWriteMask(ColorWriteChannels1);
			desc.RenderTarget[2].RenderTargetWriteMask = GetColorWriteMask(ColorWriteChannels2);
			desc.RenderTarget[3].RenderTargetWriteMask = GetColorWriteMask(ColorWriteChannels3);

			// TO-DO: considerar nuevos features de dx11.
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;

			DX::ThrowIfFailed(device->GetD3DDevice()->CreateBlendState(&desc, &m_pState));
		}

		device->GetD3DDeviceContext()->OMSetBlendState(m_pState, &BlendFactor.x, 0xFFFFFFFF);
#elif TRIO_OPENGL
		bool blendEnabled = !(ColorSourceBlend == Blend::One &&
			ColorDestinationBlend == Blend::Zero &&
			AlphaSourceBlend == Blend::One &&
			AlphaDestinationBlend == Blend::Zero);

		if (blendEnabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}

#endif
	}

#ifdef TRIO_DIRECTX

	D3D11_BLEND_OP BlendState::GetBlendOperation(BlendFunction blend)
	{
		return (D3D11_BLEND_OP)blend;
	}

	D3D11_BLEND BlendState::GetBlendOption(Blend blend, bool alpha)
	{
		switch (blend)
		{
		case Cuado::Blend::One:
			return D3D11_BLEND_ONE;

		case Cuado::Blend::Zero:
			return D3D11_BLEND_ZERO;

		case Cuado::Blend::SourceColor:
			return alpha ? D3D11_BLEND_SRC_ALPHA : D3D11_BLEND_SRC_COLOR;

		case Cuado::Blend::InverseSourceColor:
			return alpha ? D3D11_BLEND_INV_SRC_ALPHA : D3D11_BLEND_INV_SRC_COLOR;

		case Cuado::Blend::SourceAlpha:
			return D3D11_BLEND_SRC_ALPHA;

		case Cuado::Blend::InverseSourceAlpha:
			return D3D11_BLEND_INV_SRC_ALPHA;

		case Cuado::Blend::DestinationColor:
			return alpha ? D3D11_BLEND_DEST_ALPHA : D3D11_BLEND_DEST_COLOR;

		case Cuado::Blend::InverseDestinationColor:
			return alpha ? D3D11_BLEND_INV_DEST_ALPHA : D3D11_BLEND_INV_DEST_COLOR;

		case Cuado::Blend::DestinationAlpha:
			return D3D11_BLEND_DEST_ALPHA;

		case Cuado::Blend::InverseDestinationAlpha:
			return D3D11_BLEND_INV_DEST_ALPHA;

		case Cuado::Blend::BlendFactor:
			return D3D11_BLEND_BLEND_FACTOR;

		case Cuado::Blend::InverseBlendFactor:
			return D3D11_BLEND_INV_BLEND_FACTOR;

		case Cuado::Blend::SourceAlphaSaturation:
			return D3D11_BLEND_SRC_ALPHA_SAT;

		default:
			return D3D11_BLEND_ZERO;
		}
	}

	uint8_t BlendState::GetColorWriteMask(ColorWriteChannels mask)
	{
		return  ((mask & ColorWriteChannels::Red) == ColorWriteChannels::Red ? D3D11_COLOR_WRITE_ENABLE_RED : 0) |
			((mask & ColorWriteChannels::Green) == ColorWriteChannels::Green ? D3D11_COLOR_WRITE_ENABLE_GREEN : 0) |
			((mask & ColorWriteChannels::Blue) == ColorWriteChannels::Blue ? D3D11_COLOR_WRITE_ENABLE_BLUE : 0) |
			((mask & ColorWriteChannels::Alpha) == ColorWriteChannels::Alpha ? D3D11_COLOR_WRITE_ENABLE_ALPHA : 0);
	}

#endif

	void BlendState::InitStates()
	{
		if (Additive != nullptr)
			return;
		Additive = new BlendState();
		Additive->ColorSourceBlend = Blend::SourceAlpha;
		Additive->AlphaSourceBlend = Blend::SourceAlpha;
		Additive->ColorDestinationBlend = Blend::One;
		Additive->AlphaDestinationBlend = Blend::One;

		AlphaBlend = new BlendState();
		AlphaBlend->ColorSourceBlend = Blend::One;
		AlphaBlend->AlphaSourceBlend = Blend::One;
		AlphaBlend->ColorDestinationBlend = Blend::InverseSourceAlpha;
		AlphaBlend->AlphaDestinationBlend = Blend::InverseSourceAlpha;

		NonPremultiplied = new BlendState();
		NonPremultiplied->ColorSourceBlend = Blend::SourceAlpha;
		NonPremultiplied->AlphaSourceBlend = Blend::SourceAlpha;
		NonPremultiplied->ColorDestinationBlend = Blend::InverseSourceAlpha;
		NonPremultiplied->AlphaDestinationBlend = Blend::InverseSourceAlpha;

		Opaque = new BlendState();
		Opaque->ColorSourceBlend = Blend::One;
		Opaque->AlphaSourceBlend = Blend::One;
		Opaque->ColorDestinationBlend = Blend::Zero;
		Opaque->AlphaDestinationBlend = Blend::Zero;
	}

}