#include "stdafx.h"
#include "StringPool.h"

#include "../Utils/StringUtility.h"

namespace BrunoFX
{
	StringPool::StringPool(TrioMem::Allocator * allocator) : stringArray(allocator)
	{
	}

	StringPool::~StringPool()
	{
		for (int i = 0; i < stringArray.GetSize(); i++) {
			free((void *)stringArray[i]);
			stringArray[i] = nullptr;
		}
	}

	const char* StringPool::AddString(const char* string)
	{
		for (int i = 0; i < stringArray.GetSize(); i++) {
			if (TrioUtils::StringUtility::Equal(stringArray[i], string)) return stringArray[i];
		}
#if _MSC_VER
		const char * dup = _strdup(string);
#else
		const char * dup = strdup(string);
#endif
		stringArray.PushBack(dup);
		return dup;
	}

	// @@ From mprintf.cpp
	static char *mprintf_valist(int size, const char *fmt, va_list args) {
		ASSERT(size > 0);
		char *res = NULL;
		va_list tmp;

		while (1) {
			res = new char[size];
			if (!res) return NULL;

			va_copy(tmp, args);
			int len = vsnprintf(res, size, fmt, tmp);
			va_end(tmp);

			if ((len >= 0) && (size >= len + 1)) {
				break;
			}

			delete[] res;

			if (len > -1) {
				size = len + 1;
			}
			else {
				size *= 2;
			}
		}

		return res;
	}

	const char * StringPool::AddStringFormatList(const char * format, va_list args) {
		va_list tmp;
		va_copy(tmp, args);
		const char * string = mprintf_valist(256, format, tmp);
		va_end(tmp);

		for (int i = 0; i < stringArray.GetSize(); i++) {
			if (TrioUtils::StringUtility::Equal(stringArray[i], string)) {
				delete[] string;
				return stringArray[i];
			}
		}

		stringArray.PushBack(string);
		return string;
	}

	const char * StringPool::AddStringFormat(const char * format, ...) {
		va_list args;
		va_start(args, format);
		const char * string = AddStringFormatList(format, args);
		va_end(args);

		return string;
	}

	bool StringPool::GetContainsString(const char* string) const
	{
		for (int i = 0; i < stringArray.GetSize(); i++) {
			if (TrioUtils::StringUtility::Equal(stringArray[i], string)) return true;
		}
		return false;
	}
}