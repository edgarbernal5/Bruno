#include "brpch.h"
#include "UUID.h"

namespace Bruno
{
	std::random_device UUID::g_RandomDevice{};
	std::mt19937_64 UUID::eng{ (g_RandomDevice()) };
	std::uniform_int_distribution<uint64_t> UUID::g_UniformDistribution{};

	UUID::UUID()
		: m_UUID(g_UniformDistribution(eng))
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}

	UUID::UUID(const UUID& other)
		: m_UUID(other.m_UUID)
	{
	}
}
