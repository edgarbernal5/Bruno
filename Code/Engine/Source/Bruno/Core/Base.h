#pragma once

namespace Bruno
{
	void InitializeCore();
	void ShutdownCore();
}

#include "Assert.h"

#define BR_DEFINITION_FLAG_FROM_ENUM(flag) \
	inline constexpr enum flag operator |(const enum flag selfValue, const enum flag inValue) \
	{ \
		return (enum flag)(uint32_t(selfValue) | uint32_t(inValue)); \
	} \
	inline constexpr enum flag operator &(const enum flag selfValue, const enum flag inValue) \
	{ \
		return (enum flag)(uint32_t(selfValue) & uint32_t(inValue)); \
	}