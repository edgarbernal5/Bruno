// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#if TRIO_DIRECTX
#pragma comment(lib, "TrioAPI.lib")
#elif TRIO_OPENGL
//
//
//#include <GL/glew.h>
//#include <GL/wglew.h>
//
//#include <GL/gl.h>
//#include <GL/glu.h>

#pragma comment(lib, "glew32.lib")
//#pragma comment(lib, "glew32s.lib")
//#pragma comment(lib, "opengl32.lib")

#pragma comment(lib, "TrioAPIGL.lib")
#endif
// TODO: reference additional headers your program requires here
