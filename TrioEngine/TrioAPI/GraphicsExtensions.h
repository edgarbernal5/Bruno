#pragma once

//#include "TrioAPI.h"

namespace Cuado {

#if TRIO_OPENGL
	void CheckGLError(char* funcName, char* fileName, int line, char* customMessage = NULL);
#endif
}