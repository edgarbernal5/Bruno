#pragma once

#include "TrioAPI.h"

#include "DeviceFormats.h"

namespace Cuado
{
	enum class SurfaceType
	{
		TextureType,
		RenderTarget,
		SwapChainRenderTarget,
	};

	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL Texture
	{
	public:
		Texture();
		virtual ~Texture();

		friend class TextureCollection;
	protected:
		Texture(GraphicsDevice* device, SurfaceFormat format);

#ifdef TRIO_DIRECTX
		ID3D11ShaderResourceView*	m_pShaderResourceView;
		ID3D11Resource*				m_pTexture;

		ID3D11ShaderResourceView*	GetShaderResourceView() { return m_pShaderResourceView; }
#elif TRIO_OPENGL
		GLenum 						m_glFormat;
		GLint 						m_glInternalFormat;
		GLenum 						m_glTarget;
		GLuint						m_glTexture;
		GLenum 						m_glType;
#endif
		int							GetPitch(int width);

		GraphicsDevice*				m_pDevice;
		SurfaceFormat				m_eFormat;

		uint32_t					m_uLevelCount;
		uint32_t					m_ArraySize;

		bool						m_createUAV;

#if TRIO_OPENGL
		void GetGLFormat(SurfaceFormat format, GLint& glinternalFormat, GLenum& glformat, GLenum& gltype);
#endif
	};

}