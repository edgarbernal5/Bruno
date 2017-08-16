#include "stdafx.h"
#include "GraphicsCapabilities.h"

#include "GraphicsDevice.h"

namespace Cuado
{
	GraphicsCapabilities::GraphicsCapabilities()
	{
	}


	GraphicsCapabilities::~GraphicsCapabilities()
	{
	}


	void GraphicsCapabilities::Initialize(GraphicsDevice* device)
	{
#ifdef OPENGL
		char* extensions = (char*)glGetString(GL_EXTENSIONS);

		vector<string> elems;
		stringstream ss(extensions);
		string item;
		while (std::getline(ss, item, ' '))
		{
			elems.push_back(item);
		}
#endif

		m_supportsNonPowerOfTwo = GetNonPowerOfTwo(device);

#ifdef OPENGL
		m_supportsTextureFilterAnisotropic = SearchExtension(elems, "GL_EXT_texture_filter_anisotropic");
#else
		m_supportsTextureFilterAnisotropic = true;
#endif

#ifdef GLES
		m_supportsDepth24 = SearchExtension(elems, "GL_OES_depth24");
		m_supportsPackedDepthStencil = SearchExtension(elems, "GL_OES_packed_depth_stencil");
		m_supportsDepthNonLinear = SearchExtension(elems, "GL_NV_depth_nonlinear");
		m_supportsTextureMaxLevel = SearchExtension(elems, "GL_APPLE_texture_max_level");
#else
		m_supportsDepth24 = true;
		m_supportsPackedDepthStencil = true;
		m_supportsDepthNonLinear = false;
		m_supportsTextureMaxLevel = true;
#endif

#if DIRECTX
		m_supportsDxt1 = true;
		m_supportsS3tc = true;
		m_supportsPvrtc = false;
		m_supportsEtc1 = false;
		m_supportsAtitc = false;
#elif OPENGL

		m_supportsS3tc = SearchExtension(elems, "GL_EXT_texture_compression_s3tc") ||
			SearchExtension(elems, "GL_OES_texture_compression_S3TC") ||
			SearchExtension(elems, "GL_EXT_texture_compression_dxt3") ||
			SearchExtension(elems, "GL_EXT_texture_compression_dxt5");

		m_supportsDxt1 = m_supportsS3tc || SearchExtension(elems, "GL_EXT_texture_compression_dxt1");
		m_supportsPvrtc = SearchExtension(elems, "GL_IMG_texture_compression_pvrtc");
		m_supportsEtc1 = SearchExtension(elems, "GL_OES_compressed_ETC1_RGB8_texture");
		m_supportsAtitc = SearchExtension(elems, "GL_ATI_texture_compression_atitc") ||
			SearchExtension(elems, "GL_AMD_compressed_ATC_texture");
#endif

#ifdef OPENGL
		m_supportsBlitFramebuffer = true;
		m_supportsInvalidateFramebuffer = false;
#ifdef GLES
		m_supportsFramebufferObjectARB = true; // always supported on GLES 2.0+
		m_supportsFramebufferObjectEXT = false;
#else
		m_supportsFramebufferObjectARB = SearchExtension(elems, "GL_ARB_framebuffer_object");
		m_supportsFramebufferObjectEXT = SearchExtension(elems, "GL_EXT_framebuffer_object");
#endif
#endif
	}

	bool GraphicsCapabilities::GetNonPowerOfTwo(GraphicsDevice* device)
	{
#ifdef OPENGL
		return true; // device._maxTextureSize >= 8192;
#else
		return true;
#endif
	}

#ifdef OPENGL
	bool GraphicsCapabilities::SearchExtension(vector<string>& extensions, string item)
	{
		vector<string>::iterator it = find(extensions.begin(), extensions.end(), item);
		if (it != extensions.end())
			return true;
		return false;
	}
#endif
}