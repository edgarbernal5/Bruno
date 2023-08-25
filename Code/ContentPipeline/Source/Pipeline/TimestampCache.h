#pragma once

#include <string>
#include <map>
#include <filesystem>

namespace Bruno
{
	class TimestampCache
	{
	public:
		TimestampCache() = default;

		std::filesystem::file_time_type GetTimestamp(const std::wstring& filename);
		bool FileExists(const std::wstring& filename);
		void Remove(const std::wstring& filename);

	private:
		std::map<std::wstring, bool> m_existsCache;
		std::map<std::wstring, std::filesystem::file_time_type> m_timestampCache;
	};
}
