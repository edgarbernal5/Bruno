#pragma once

#include "TrioApiDecl.h"

#include "SurfaceType.h"
#include "SurfaceFormat.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class TRIOAPI_DECL Texture
	{
	public:
		Texture();
		virtual ~Texture();

		inline SurfaceFormat GetFormat() { return m_eFormat; }
		inline uint32_t GetLevelCount() { return m_uLevelCount; }
		
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

		bool						m_bCreateUAV;
	};

}
