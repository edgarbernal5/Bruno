#pragma once

#include "TrioApiRequisites.h"

#include "DepthFormat.h"
#include "DepthStencilViewFlags.h"
#include "DepthStencilViewDimension.h"

namespace BrunoEngine
{
	class GraphicsDevice;

	class BRUNO_API_EXPORT DepthStencilBuffer
	{
	public:
		DepthStencilBuffer();
		DepthStencilBuffer(GraphicsDevice* device, int width, int height, DepthFormat format);

#ifdef BRUNO_DIRECTX
		DepthStencilBuffer(GraphicsDevice* device, int width, int height, DepthFormat format, DepthStencilViewDimension viewDimension, DepthStencilViewFlags viewFlags);
#endif

		~DepthStencilBuffer();

#ifdef BRUNO_DIRECTX
		ID3D11DepthStencilView* GetDepthStencilView() { return m_depthStencilView.Get(); }
#endif
		inline DepthFormat GetDepthFormat() { return m_depthFormat; }

		friend class GraphicsDevice;
	protected:

#ifdef BRUNO_DIRECTX
		Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_depthStencilTexture;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceView;
#elif BRUNO_OPENGL
		GLuint								m_glDepthBuffer;
		GLuint								m_glStencilBuffer;
#endif

		DepthFormat							m_depthFormat;
		GraphicsDevice*						m_device;
	};
}
