#include "stdafx.h"
#include "StringHelpers.h"


#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <locale>
#include <algorithm>

#pragma warning(disable : 4996)

namespace TrioFX
{
	int String_PrintfArgList(char* buffer, int bufferSize, const char* format, va_list args)
	{
		va_list tmp;
		va_copy(tmp, args);

#if _MSC_VER >= 1400
		int n = vsnprintf_s(buffer, bufferSize, _TRUNCATE, format, tmp);
#else
		int n = vsnprintf(buffer, bufferSize, format, tmp);
#endif

		va_end(tmp);

		if (n < 0 || n > bufferSize) return -1;
		return n;
	}

	int String_Printf(char* buffer, int bufferSize, const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		int count = String_PrintfArgList(buffer, bufferSize, format, args);

		va_end(args);
		return count;
	}

	bool String_Equal(const char* first, const char* second)
	{
		if (first == second) return true;
		if (first == nullptr || second == nullptr) return false;

		return std::strcmp(first, second) == 0;
	}

	bool String_EqualNoCase(const char* first, const char* second)
	{
		if (first == second) return true;
		if (first == nullptr || second == nullptr) return false;

#if _MSC_VER
		return _stricmp(first, second) == 0;
#else
		return strcasecmp(first, second) == 0;
#endif
	}

	double String_ToDouble(const char* buffer, char** end)
	{
		return std::strtod(buffer, end);
	}

	int String_ToInteger(const char* buffer, char** end)
	{
		/*long value = std::strtol(buffer, end, 0);

		if (value > INT_MAX)
		{
			return INT_MAX;
		}
		if (value < INT_MIN)
		{
			return INT_MIN;
		}

		return static_cast<int>(value);*/

		return strtol(buffer, end, 10);
	}

	int String_FormatFloat(char* buffer, int bufferSize, float value)
	{
		return String_Printf(buffer, bufferSize, "%f", value);
	}
}