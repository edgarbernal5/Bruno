#pragma once

#if TRIO_OPENGL
#include "TrioAPI.h"


namespace Cuado
{
	class TRIOAPI_DLL OpenGLContext
	{
	public:
		OpenGLContext();
		~OpenGLContext();

		bool CreateContext(HWND hwnd);
		void SwapBuffers();
	protected:
		HGLRC m_hrc; // Rendering context
		HDC m_hdc; // Device context
		HWND m_hwnd; // Window identifier
	};
}
#endif