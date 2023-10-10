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
			Bruno::Log::PrintAssertMessage(__FILE__, __LINE__, "Assertion failed", __VA_ARGS__); \
			BR_DEBUG_BREAK; \
		}

	#define BR_ASSERT_ERROR(message) \
		{ \
			Bruno::Log::PrintAssertMessage(__FILE__, __LINE__, "Assertion failed", message); \
			BR_DEBUG_BREAK; \
		}
#else
	#define BR_ASSERT(expression, ...)
	#define BR_ASSERT_ERROR(message)
#endif