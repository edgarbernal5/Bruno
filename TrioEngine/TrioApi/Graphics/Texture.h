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

		inline SurfaceFormat GetFormat() { return m_format; }
		inline uint32_t GetLevelCount() { return m_levelCount; }
		
		friend class TextureCollection;
	protected:
		Texture(GraphicsDevice* device, SurfaceFormat format);

#ifdef TRIO_DIRECTX
		ID3D11ShaderResourceView*	m_shaderResourceView;
		ID3D11Resource*				m_texture;

		ID3D11ShaderResourceView*	GetShaderResourceView() { return m_shaderResourceView; }
#elif TRIO_OPENGL
		GLenum 						m_glFormat;
		GLint 						m_glInternalFormat;
		GLenum 						m_glTarget;
		GLuint						m_glTexture;
		GLenum 						m_glType;
#endif
		int							GetPitch(int width);

		GraphicsDevice*				m_device;
		SurfaceFormat				m_format;

		uint32_t					m_levelCount;
		uint32_t					m_arraySize;

		bool						m_createUAV;
	};

}
