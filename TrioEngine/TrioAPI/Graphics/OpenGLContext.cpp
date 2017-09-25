#include "stdafx.h"
#include "OpenGLContext.h"

#if TRIO_OPENGL
namespace Cuado
{
	
	OpenGLContext::OpenGLContext()
	{
		
	}
	OpenGLContext::~OpenGLContext()
	{
		wglMakeCurrent(m_hdc, 0); // Remove the rendering context from our device context
		wglDeleteContext(m_hrc); // Delete our rendering context

		ReleaseDC(m_hwnd, m_hdc); // Release the device context from our window
	}

	bool OpenGLContext::CreateContext(HWND hwnd)
	{
		m_hwnd = hwnd; // Set the HWND for our window

		m_hdc = GetDC(m_hwnd); // Get the device context for our window

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAlphaBits = 8;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
		DescribePixelFormat(m_hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

		int result;
		result = SetPixelFormat(m_hdc, pixelFormat, &pfd);

		HGLRC tempContext = wglCreateContext(m_hdc);
		result = wglMakeCurrent(m_hdc, tempContext);


		GLenum error = glewInit(); // Enable GLEW
		if (error != GLEW_OK) // If GLEW fails
		{
			return false;
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};

		if (wglewIsSupported("WGL_ARB_create_context") == 1)
		{
			m_hrc = wglCreateContextAttribsARB(m_hdc, 0, attribs);
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tempContext);
			wglMakeCurrent(m_hdc, m_hrc);
		}
		else
		{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
			m_hrc = tempContext;
		}

		//Checking GL version
		const GLubyte *GLVersionString = glGetString(GL_VERSION);

		//Or better yet, use the GL3 way to get the version number
		int OpenGLVersion[2];
		glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
		glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

		if (!m_hrc) return false;

		//if (wglSwapIntervalEXT != nullptr) {
		//	wglSwapIntervalEXT(0);
		//}
		return true;
	}

	void OpenGLContext::SwapBuffers()
	{
		::SwapBuffers(m_hdc);
	}
}
#endif