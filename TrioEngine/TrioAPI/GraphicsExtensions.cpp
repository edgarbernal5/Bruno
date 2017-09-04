#include "stdafx.h"
#include "GraphicsExtensions.h"

#include <iostream>
#include <sstream>
#include <string>

namespace Cuado
{

#if TRIO_OPENGL
	void CheckGLError(char* funcName, char* fileName, int line, char* customMessage)
	{
		GLenum errorGL = glGetError();
		if (errorGL == GL_NO_ERROR) {
			return;
		}
		std::stringstream ss;

		ss << funcName << " at file " << fileName << " at line " << std::endl;
		ss << line << std::endl << std::endl;

		while (errorGL != GL_NO_ERROR)
		{
			std::string error;

			switch (errorGL)
			{
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			case GL_STACK_OVERFLOW:  error = "GL_STACK_OVERFLOW";  break;
			case GL_STACK_UNDERFLOW:  error = "GL_STACK_UNDERFLOW";  break;
			}

			ss << "\t" << error << std::endl << std::endl;

			errorGL = glGetError();
		}

		if (customMessage) {
			ss << customMessage;
		}
		throw std::runtime_error(ss.str());
	}
#endif
}