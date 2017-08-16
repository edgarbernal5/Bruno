
#include "stdafx.h"
#include "Utilities.h"

namespace Cuado
{
	int ComputeHash(const char * data, int size)
	{
		const int p = 16777619;
		int hash = (int)2166136261;

		for (int i = 0; i < size; i++)
			hash = (hash ^ data[i]) * p;

		hash += hash << 13;
		hash ^= hash >> 7;
		hash += hash << 3;
		hash ^= hash >> 17;
		hash += hash << 5;
		return hash;
	}

	int ComputeHash(uint8_t * data, int size)
	{
		const int p = 16777619;
		int hash = (int)2166136261;

		for (int i = 0; i < size; i++)
			hash = (hash ^ data[i]) * p;

		hash += hash << 13;
		hash ^= hash >> 7;
		hash += hash << 3;
		hash ^= hash >> 17;
		hash += hash << 5;
		return hash;
	}

	int ComputeHash(uint8_t * data, size_t size)
	{
		const int p = 16777619;
		int hash = (int)2166136261;

		for (int i = 0; i < size; i++)
			hash = (hash ^ data[i]) * p;

		hash += hash << 13;
		hash ^= hash >> 7;
		hash += hash << 3;
		hash ^= hash >> 17;
		hash += hash << 5;
		return hash;
	}
}