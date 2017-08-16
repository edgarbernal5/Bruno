#pragma once

#include "TrioAPI.h"

#include "Texture2D.h"
#include "IRenderTarget.h"

#include "ResourceEnums.h"

namespace Cuado
{
	class TRIOAPI_DLL RenderTarget2D : public Texture2D, public IRenderTarget
	{
	public:
		RenderTarget2D();
		RenderTarget2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipMap, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, uint32_t preferredMultiSampleCount, uint32_t preferredMultiSampleQuality, RenderTargetUsage usage, bool shared);
		~RenderTarget2D();

#ifdef TRIO_DIRECTX
		ID3D11RenderTargetView* GetRenderTargetView(uint32_t arraySlice);
#endif
		DepthStencilBuffer* GetDepthStencilBuffer();

		int GetHeight();
		int GetWidth();
		RenderTargetUsage GetRenderTargetUsage();

		friend class GraphicsDevice;
	private:
#ifdef TRIO_DIRECTX
		ID3D11RenderTargetView* m_RenderTargetView;
#elif TRIO_OPENGL
		GLuint m_glColorBuffer;
#endif
		DepthStencilBuffer* m_DepthBuffer;
		
		RenderTargetUsage m_RenderTargetUsage;
		DepthFormat m_DepthFormat;
	};

}

