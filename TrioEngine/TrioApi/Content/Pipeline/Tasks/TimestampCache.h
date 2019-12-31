#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>
#include <chrono>

namespace TrioEngine
{
	class TimestampCache
	{
	public:
		TimestampCache() = default;
		
	private:
		std::map<std::string, std::chrono::system_clock::time_point> m_cache;
	};
}