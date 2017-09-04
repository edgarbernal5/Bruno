#ifndef __GRAPHICS_EXTENSIONS_H_
#define __GRAPHICS_EXTENSIONS_H_

#include "TrioAPI.h"

namespace Cuado {

#if TRIO_OPENGL
	void TRIOAPI_DLL CheckGLError(char* funcName, char* fileName, int line, char* customMessage = NULL);

#if _DEBUG
#define CHECK_GL_ERROR_MSG(funcName, customMessage) Cuado::CheckGLError(#funcName, __FILE__, __LINE__, customMessage)
#define CHECK_GL_ERROR(FUNCNAME) Cuado::CheckGLError(#FUNCNAME, __FILE__, __LINE__)
#else
#define CHECK_GL_ERROR_MSG(funcName, customMessage)
#define CHECK_GL_ERROR(funcName)
#endif
	
#endif
}

#endif