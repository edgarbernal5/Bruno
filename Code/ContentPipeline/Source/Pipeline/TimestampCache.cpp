#include "TimestampCache.h"

namespace Bruno
{
	std::filesystem::file_time_type TimestampCache::GetTimestamp(const std::wstring& filename)
	{
		auto it = m_timestampCache.find(filename);
		if (it == m_timestampCache.end()) {
			std::filesystem::path pathToFilename(filename);
			auto fileTimestamp = std::filesystem::last_write_time(pathToFilename);

			m_timestampCache[filename] = fileTimestamp;
			return fileTimestamp;
		}
		return it->second;
	}

	bool TimestampCache::FileExists(const std::wstring& filename)
	{
		auto it = m_existsCache.find(filename);
		if (it == m_existsCache.end()) {
			bool exists = std::filesystem::exists(filename);
			m_existsCache[filename] = exists;
			return exists;
		}
		return it->second;
	}

	void TimestampCache::Remove(const std::wstring& filename)
	{
		m_timestampCache.erase(filename);
		m_existsCache.erase(filename);
	}
}