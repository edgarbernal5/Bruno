#include "brpch.h"
#include "StringHelpers.h"

namespace Bruno
{
	std::wstring StringToWString(const std::string& str)
	{
		return std::wstring(str.cbegin(), str.cend());
	}

	std::string WStringToString(const std::wstring& str)
	{
		return std::string(str.cbegin(), str.cend());
	}
}