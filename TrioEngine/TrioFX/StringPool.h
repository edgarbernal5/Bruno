#pragma once

#include "CommonHeader.h"

#include <string>
#include <set>

using namespace std;

namespace CuadoFX
{
	class StringPool
	{

	public:

		StringPool();

		const char* AddString(const char* string);
		bool GetContainsString(const char* string) const;

	private:

		set<string> m_strings;

	};

}