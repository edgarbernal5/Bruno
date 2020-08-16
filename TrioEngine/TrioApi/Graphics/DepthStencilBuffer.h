#pragma once

#include "TrioApiRequisites.h"

#include "DepthFormat.h"
#include "DepthStencilViewFlags.h"
#include "DepthStencilViewDimension.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class TRIO_API_EXPORT DepthStencilBuffer
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
	protected:

#ifdef TRIO_DIRECTX
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_depthStencil;

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
