#pragma once

#include "Bruno/Core/Log.h"

#if BR_PLATFORM_WINDOWS
	#define BR_DEBUG_BREAK __debugbreak()
#else
	#define BR_DEBUG_BREAK
#endif

#if BR_DEBUG
	#define BR_ASSERT(expression, ...) \
		if (expression){ } \
		else { \
			Bruno::Log::PrintAssertMessage("Assertion fails: "#__VA_ARGS__, __FILE__, __LINE__); \
			BR_DEBUG_BREAK; \
		}
#else
	#define BR_ASSERT(expression, ...)
#endif