#include "brpch.h"
#include "UUID.h"

namespace Bruno
{
	std::random_device UUID::g_RandomDevice{};
	std::mt19937_64 UUID::g_mtEng{ (g_RandomDevice()) };
	std::uniform_int_distribution<uint64_t> UUID::g_UniformDistribution{};

	UUID::UUID()
		: m_uuid(g_UniformDistribution(g_mtEng))
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_uuid(uuid)
	{
	}

	UUID::UUID(const UUID& other)
		: m_uuid(other.m_uuid)
	{
	}
}
