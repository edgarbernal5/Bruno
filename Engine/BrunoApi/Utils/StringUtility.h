#pragma once

#include <cstdarg>

namespace TrioUtils
{
	class StringUtility
	{
	public:
		static bool Equal(const char* first, const char* second);
		static bool EqualNoCase(const char* first, const char* second);

		static int FormatFloat(char* buffer, int bufferSize, float value);
		static int Printf(char* buffer, int bufferSize, const char* format, ...);
		static int PrintfArgList(char* buffer, int bufferSize, const char* format, va_list args);
		static double ToDouble(const char* buffer, char** end);
		static int ToInteger(const char* buffer, char** end);
		static void ToWideString(const std::string& source, std::wstring& dest);
	};
}