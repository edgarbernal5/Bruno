#include "stdafx.h"
#include "StringUtility.h"

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <locale>
#include <algorithm>

#pragma warning(disable : 4996)

namespace TrioUtils
{
	int StringUtility::PrintfArgList(char* buffer, int bufferSize, const char* format, va_list args)
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

	int StringUtility::Printf(char* buffer, int bufferSize, const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		int count = StringUtility::PrintfArgList(buffer, bufferSize, format, args);

		va_end(args);
		return count;
	}

	bool StringUtility::Equal(const char* first, const char* second)
	{
		if (first == second) return true;
		if (first == nullptr || second == nullptr) return false;

		return std::strcmp(first, second) == 0;
	}

	bool StringUtility::EqualNoCase(const char* first, const char* second)
	{
		if (first == second) return true;
		if (first == nullptr || second == nullptr) return false;

#if _MSC_VER
		return _stricmp(first, second) == 0;
#else
		return strcasecmp(first, second) == 0;
#endif
	}

	double StringUtility::ToDouble(const char* buffer, char** end)
	{
		return std::strtod(buffer, end);
	}

	int StringUtility::ToInteger(const char* buffer, char** end)
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

	int StringUtility::FormatFloat(char* buffer, int bufferSize, float value)
	{
		return StringUtility::Printf(buffer, bufferSize, "%f", value);
	}

	void StringUtility::ToWideString(const std::string& source, std::wstring& dest)
	{
		dest.assign(source.begin(), source.end());
	}
}