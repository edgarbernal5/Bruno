#pragma once

namespace Bruno
{
	void InitializeCore();
	void ShutdownCore();
}

#include "Assert.h"

#define BR_DEFINITION_FLAG_FROM_ENUM(Flag) \
	inline constexpr Flag operator |(const Flag selfValue, const Flag inValue) \
	{ \
		return static_cast<Flag>(static_cast<uint8_t>(selfValue) | static_cast<uint8_t>(inValue)); \
	} \
	inline constexpr Flag operator &(const Flag selfValue, const Flag inValue) \
	{ \
		return static_cast<Flag>(static_cast<uint8_t>(selfValue) & static_cast<uint8_t>(inValue)); \
	}