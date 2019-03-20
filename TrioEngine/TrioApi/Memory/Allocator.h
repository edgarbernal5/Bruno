#pragma once

#include <stdlib.h> // malloc
#include <new> // for placement new

namespace TrioMem
{
	class Allocator
	{
	public:
		template <typename T> T * New() {
			return (T *)malloc(sizeof(T));
		}
		template <typename T> T * New(size_t count) {
			return (T *)malloc(sizeof(T) * count);
		}
		template <typename T> void Delete(T * ptr) {
			free((void *)ptr);
		}
		template <typename T> T * Realloc(T * ptr, size_t count) {
			return (T *)realloc(ptr, sizeof(T) * count);
		}
	};
}