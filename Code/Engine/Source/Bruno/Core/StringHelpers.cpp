#include "brpch.h"
#include "StringHelpers.h"

namespace Bruno
{
	std::wstring StringToWString(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}
}