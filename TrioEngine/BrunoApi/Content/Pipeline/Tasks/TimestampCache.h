#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>
#include <chrono>
#include <filesystem>

namespace BrunoEngine
{
	class TimestampCache
	{
	public:
		TimestampCache() = default;
		
		std::filesystem::file_time_type GetTimestamp(std::string filename);
		void Remove(const std::string& filename);
	private:
		std::map<std::string, std::filesystem::file_time_type> m_cache;
	};
}