#pragma once

#include "TrioApiDecl.h"

#include "../DataTypes/Array.h"

namespace TrioFX
{
	class StringPool
	{
	public:
		StringPool(TrioMem::Allocator * allocator);
		~StringPool();

		const char* AddString(const char* string);
		const char * AddStringFormat(const char * fmt, ...);
		const char * AddStringFormatList(const char * fmt, va_list args);
		bool GetContainsString(const char* string) const;

	private:
		TrioData::Array<const char *> stringArray;

	};

}