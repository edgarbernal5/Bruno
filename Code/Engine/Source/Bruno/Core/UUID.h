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

		operator uint64_t () { return m_UUID; }
		operator const uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;

		static std::random_device g_RandomDevice;
		static std::mt19937_64 eng;// (g_RandomDevice());
		static std::uniform_int_distribution<uint64_t> g_UniformDistribution;
	};
}

