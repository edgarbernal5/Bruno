#pragma once

#include <string>

namespace Bruno
{
	std::wstring StringToWString(const std::string& str);
	std::string WStringToString(const std::wstring& str);
}
