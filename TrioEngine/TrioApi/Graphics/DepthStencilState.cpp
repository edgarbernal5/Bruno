#include "stdafx.h"
#include "DepthStencilState.h"


namespace Vago
{
	DepthStencilState * DepthStencilState::Default = nullptr;
	DepthStencilState * DepthStencilState::DepthRead = nullptr;
	DepthStencilState * DepthStencilState::None = nullptr;

	DepthStencilState::DepthStencilState()
#ifdef TRIO_DIRECTX
		:
		m_pState(nullptr)
#endif
	{
		DepthBufferEnable = true;
        DepthBufferWriteEnable = true;
		DepthBufferFunction = CompareFunction::LessEqual;
		StencilEnable = false;
		StencilFunction = CompareFunction::Always;
		StencilPass = StencilOperation::Keep;
		StencilFail = StencilOperation::Keep;
		StencilDepthBufferFail = StencilOperation::Keep;
		TwoSidedStencilMode = false;
		CounterClockwiseStencilFunction = CompareFunction::Always;
		CounterClockwiseStencilFail = StencilOperation::Keep;
		CounterClockwiseStencilPass = StencilOperation::Keep;
		CounterClockwiseStencilDepthBufferFail = StencilOperation::Keep;
		StencilMask = INT_MAX;
		StencilWriteMask = INT_MAX;
		ReferenceStencil = 0;
	}

	DepthStencilState::~DepthStencilState()
	{
	}

	void DepthStencilState::ApplyState(GraphicsDevice * device)
	{

#ifdef TRIO_DIRECTX
		if (m_pState == nullptr)
        {
			D3D11_DEPTH_STENCIL_DESC desc;
			desc.DepthEnable = DepthBufferEnable;
			desc.DepthFunc = GetComparison(DepthBufferFunction);

			if (DepthBufferWriteEnable)
				desc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)DepthWriteMask::All;
			else
				desc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)DepthWriteMask::Zero;

			desc.StencilEnable = StencilEnable;
			desc.StencilReadMask = StencilMask;
			desc.StencilWriteMask = StencilWriteMask;

			if (TwoSidedStencilMode)
			{
				desc.BackFace.StencilFunc = GetComparison(CounterClockwiseStencilFunction);
				desc.BackFace.StencilDepthFailOp = GetStencilOp(CounterClockwiseStencilDepthBufferFail);
				desc.BackFace.StencilFailOp = GetStencilOp(CounterClockwiseStencilFail);
				desc.BackFace.StencilPassOp = GetStencilOp(CounterClockwiseStencilPass);
			}
			else
			{
				desc.BackFace.StencilFunc = GetComparison(StencilFunction);
				desc.BackFace.StencilDepthFailOp = GetStencilOp(StencilDepthBufferFail);
				desc.BackFace.StencilFailOp = GetStencilOp(StencilFail);
				desc.BackFace.StencilPassOp = GetStencilOp(StencilPass);
			}
			
			desc.FrontFace.StencilFunc = GetComparison(StencilFunction);
			desc.FrontFace.StencilDepthFailOp = GetStencilOp(StencilDepthBufferFail);
			desc.FrontFace.StencilFailOp = GetStencilOp(StencilFail);
			desc.FrontFace.StencilPassOp = GetStencilOp(StencilPass);

			DX::ThrowIfFailed(device->GetD3DDevice()->CreateDepthStencilState(&desc, &m_pState));
		}

		device->GetD3DDeviceContext()->OMSetDepthStencilState(m_pState, ReferenceStencil);
#elif OPENGL
		if (DepthBufferEnable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		GLenum func;
		switch (DepthBufferFunction)
		{
		default:
		case CompareFunction::Always:
			func = GL_ALWAYS;
			break;
		case CompareFunction::Equal:
			func = GL_EQUAL;
			break;
		case CompareFunction::Greater:
			func = GL_GREATER;
			break;
		case CompareFunction::GreaterEqual:
			func = GL_GEQUAL;
			break;
		case CompareFunction::Less:
			func = GL_LESS;
			break;
		case CompareFunction::LessEqual:
			func = GL_LEQUAL;
			break;
		case CompareFunction::Never:
			func = GL_NEVER;
			break;
		case CompareFunction::NotEqual:
			func = GL_NOTEQUAL;
			break;
		}
		glDepthFunc(func);
		glDepthMask(DepthBufferWriteEnable);
#endif
	}

#ifdef TRIO_DIRECTX
	D3D11_COMPARISON_FUNC DepthStencilState::GetComparison(CompareFunction compare)
    {
        return (D3D11_COMPARISON_FUNC)compare;
    }

	D3D11_STENCIL_OP DepthStencilState::GetStencilOp(StencilOperation op)
	{
        return (D3D11_STENCIL_OP)op;
	}

#endif
	
	void DepthStencilState::InitStates()
	{
		if (Default != nullptr)
			return;

		Default = new DepthStencilState();
		Default->DepthBufferEnable = true;
		Default->DepthBufferWriteEnable = true;
		
		DepthRead = new DepthStencilState();
		DepthRead->DepthBufferEnable = true;
		DepthRead->DepthBufferWriteEnable = false;
		
		None = new DepthStencilState();
		None->DepthBufferEnable = false;
		None->DepthBufferWriteEnable = false;
	}
}