#pragma once

#include <cstdarg>

namespace TrioUtils
{
	class StringUtility
	{
	public:
		static int PrintfArgList(char* buffer, int bufferSize, const char* format, va_list args);

		static int Printf(char* buffer, int bufferSize, const char* format, ...);

		static bool Equal(const char* first, const char* second);

		static bool EqualNoCase(const char* first, const char* second);

		static double ToDouble(const char* buffer, char** end);

		static int ToInteger(const char* buffer, char** end);

		static int FormatFloat(char* buffer, int bufferSize, float value);

		static void ToWideString(const std::string& source, std::wstring& dest);
	};
}