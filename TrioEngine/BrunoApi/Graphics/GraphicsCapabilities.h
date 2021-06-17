#pragma once

#include "TrioApiRequisites.h"
#include <vector>
#include <string>

namespace TrioEngine
{
	class GraphicsDevice;

	class GraphicsCapabilities
	{
	public:
		GraphicsCapabilities();
		~GraphicsCapabilities();

		void Initialize(GraphicsDevice* device);

		inline bool GetSupportsNonPowerOfTwo()
		{
			return m_supportsNonPowerOfTwo;
		}
		inline bool GetSupportsTextureFilterAnisotropic()
		{
			return m_supportsTextureFilterAnisotropic;
		}
		inline bool GetSupportsDepth24()
		{
			return m_supportsDepth24;
		}
	private:
		bool m_supportsNonPowerOfTwo;
		bool m_supportsTextureFilterAnisotropic;
		bool m_supportsDepth24;
		bool m_supportsPackedDepthStencil;
		bool m_supportsDepthNonLinear;
		bool m_supportsDxt1;
		bool m_supportsS3tc;
		bool m_supportsPvrtc;
		bool m_supportsEtc1;
		bool m_supportsAtitc;

#ifdef BRUNO_OPENGL
		bool m_supportsBlitFramebuffer, m_supportsFramebufferObjectARB;
		bool m_supportsInvalidateFramebuffer, m_supportsFramebufferObjectEXT;
#endif

		bool m_supportsTextureMaxLevel;

		bool GetNonPowerOfTwo(GraphicsDevice* device);

#ifdef BRUNO_OPENGL
		bool SearchExtension(std::vector<std::string>& extensions, std::string item);
#endif
	};
}