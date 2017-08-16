#pragma once


#include "TrioAPI.h"

#include "DeviceFormats.h"

namespace Cuado
{
	class TRIOAPI_DLL DepthStencilBuffer;

	class TRIOAPI_DLL IRenderTarget
	{
	public:
#ifdef TRIO_DIRECTX
		virtual ID3D11RenderTargetView * GetRenderTargetView(uint32_t arraySlice) = 0;
#endif

		virtual DepthStencilBuffer* GetDepthStencilBuffer() = 0;

		virtual int GetHeight() = 0;
		virtual int GetWidth() = 0;
		virtual RenderTargetUsage GetRenderTargetUsage() = 0;
	};
}
