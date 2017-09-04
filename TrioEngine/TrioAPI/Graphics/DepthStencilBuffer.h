#pragma once

#include "TrioAPI.h"

#include "DeviceFormats.h"
#include "ResourceEnums.h"

namespace Cuado
{
	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL DepthStencilBuffer
	{
	public:
		DepthStencilBuffer();
		DepthStencilBuffer(GraphicsDevice* device, int width, int height, DepthFormat format);

#ifdef TRIO_DIRECTX
		DepthStencilBuffer(GraphicsDevice* device, int width, int height, DepthFormat format, DepthStencilViewDimension viewDimension, DepthStencilViewFlags viewFlags);
#endif

		~DepthStencilBuffer();

#ifdef TRIO_DIRECTX
		ID3D11DepthStencilView* GetDepthStencilView() { return m_depthStencilView.Get(); }
#endif
		inline DepthFormat GetDepthFormat() { return m_depthFormat; }

		friend class GraphicsDevice;
		friend class RenderTarget2D;
	protected:

#ifdef TRIO_DIRECTX
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceView;
#elif TRIO_OPENGL
		GLuint								m_glDepthBuffer;
		GLuint								m_glStencilBuffer;
#endif

		DepthFormat							m_depthFormat;
		GraphicsDevice*						m_device;
	};
}