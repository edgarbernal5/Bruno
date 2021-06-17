#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{ 
	class IRenderTarget
	{
	public:
		virtual ~IRenderTarget() { }
		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;

#if BRUNO_DIRECTX
		virtual ID3D11RenderTargetView* GetRenderTargetView(int arraySlice) = 0;
		virtual ID3D11DepthStencilView* GetDepthStencilView() = 0;
#endif
	};
}
