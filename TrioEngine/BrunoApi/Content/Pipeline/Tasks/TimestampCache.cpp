#include "stdafx.h"
#include "TimestampCache.h"

#include <filesystem>

namespace BrunoEngine
{
	std::filesystem::file_time_type TimestampCache::GetTimestamp(std::string filename)
	{
		auto it = m_cache.find(filename);
		if (it == m_cache.end()) {
			std::filesystem::path pathToFilename(filename);
			auto fileTimestamp = std::filesystem::last_write_time(pathToFilename);

			m_cache[filename] = fileTimestamp;
			return fileTimestamp;
		}
		return it->second;
	}

	void TimestampCache::Remove(const std::string& filename)
	{
		m_cache.erase(filename);
	}
}