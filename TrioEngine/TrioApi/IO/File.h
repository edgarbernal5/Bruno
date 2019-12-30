#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioIO
{
	class File
	{
	public:
		static bool Delete(const std::string& filename);
		static bool Exist(const std::string& path);
	};
}