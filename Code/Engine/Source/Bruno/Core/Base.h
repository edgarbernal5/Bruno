#pragma once

#include <functional>

namespace Bruno
{
	void InitializeCore();
	void ShutdownCore();
	
	template <class T>
	inline void HashCombine(size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
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