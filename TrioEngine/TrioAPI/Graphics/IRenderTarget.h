#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{ 
	class IRenderTarget
	{
	public:
		virtual ~IRenderTarget() { }
		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;

#if TRIO_DIRECTX
		virtual ID3D11RenderTargetView* GetRenderTargetView(int arraySlice) = 0;
		virtual ID3D11DepthStencilView* GetDepthStencilView() = 0;
#endif
	};
}
