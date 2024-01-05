#pragma once

#include <random>

namespace Bruno
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& other);

		operator uint64_t () { return m_uuid; }
		operator const uint64_t() const { return m_uuid; }

		bool operator== (const UUID& other) const
		{
			return m_uuid == other.m_uuid;
		}
	private:
		uint64_t m_uuid;

		static std::random_device g_RandomDevice;
		static std::mt19937_64 g_mtEng;
		static std::uniform_int_distribution<uint64_t> g_UniformDistribution;
	};
}

namespace std
{
	template <>
	struct hash<Bruno::UUID>
	{
		std::size_t operator()(const Bruno::UUID& key) const
		{
			//return std::hash<uint64_t>{}(key);
			return key;
		}
	};
}