#include "stdafx.h"
#include "RasterizerState.h"

#include "GraphicsDevice.h"

namespace TrioEngine
{
	RasterizerState* RasterizerState::CullClockwise = nullptr;
	RasterizerState* RasterizerState::CullCounterClockwise = nullptr;
	RasterizerState* RasterizerState::CullNone = nullptr;

	RasterizerState::RasterizerState()
#ifdef TRIO_DIRECTX
		:
		m_pState(nullptr)
#endif
	{
		CullMode = CullMode::CullCounterClockwiseFace;
		FillMode = FillMode::Solid;
		DepthBias = 0;
		MultiSampleAntiAlias = true;
		ScissorTestEnable = false;
		SlopeScaleDepthBias = 0;
		DepthClipEnable = true;
		DepthBiasClamp = 0.0f;
	}

	RasterizerState::~RasterizerState()
	{
	}

	void RasterizerState::ApplyState(GraphicsDevice* device)
	{
#ifdef TRIO_DIRECTX
		if (m_pState == nullptr)
        {
			D3D11_RASTERIZER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
			
			switch (CullMode)
			{
			case CullMode::None:
				desc.CullMode = D3D11_CULL_NONE;
				break;
			case CullMode::CullClockwiseFace:
				desc.CullMode = D3D11_CULL_FRONT;
				break;
			case CullMode::CullCounterClockwiseFace:
				desc.CullMode = D3D11_CULL_BACK;
				break;
			}

			desc.ScissorEnable = ScissorTestEnable;
			desc.MultisampleEnable = MultiSampleAntiAlias;
			desc.DepthBias = (int)DepthBias;
			desc.SlopeScaledDepthBias = SlopeScaleDepthBias;
			desc.DepthClipEnable = DepthClipEnable;

			if (FillMode == FillMode::WireFrame)
				desc.FillMode = D3D11_FILL_WIREFRAME;
			else
				desc.FillMode = D3D11_FILL_SOLID;

			desc.FrontCounterClockwise = false;
			desc.AntialiasedLineEnable = false;

			desc.DepthBiasClamp = DepthBiasClamp;
			DX::ThrowIfFailed(
				device->GetD3DDevice()->CreateRasterizerState(&desc, &m_pState)
			);
		}

		device->GetD3DDeviceContext()->RSSetState(m_pState);
#elif TRIO_OPENGL
		// When rendering offscreen the faces change order.
		//bool offscreen = device.IsRenderTargetBound;
		bool offscreen = false;


		if (CullMode == CullMode::None)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			if (CullMode == CullMode::CullClockwiseFace)
			{
				if (offscreen)
					glFrontFace(GL_CW);
				else
					glFrontFace(GL_CCW);
			}
			else
			{
				if (offscreen)
					glFrontFace(GL_CCW);
				else
					glFrontFace(GL_CW);
			}
		}

		if (FillMode == FillMode::Solid)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	}

	void RasterizerState::InitStates()
	{
		if (CullClockwise != nullptr)
			return;

		CullClockwise = new RasterizerState();
		CullClockwise->CullMode = CullMode::CullClockwiseFace;
		
		CullCounterClockwise = new RasterizerState();
		CullCounterClockwise->CullMode = CullMode::CullCounterClockwiseFace;
		
		CullNone = new RasterizerState();
		CullNone->CullMode = CullMode::None;
	}
}