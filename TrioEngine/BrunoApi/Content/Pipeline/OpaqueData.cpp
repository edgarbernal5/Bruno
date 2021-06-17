#include "stdafx.h"
#include "OpaqueData.h"

namespace BrunoEngine
{
	OpaqueData::OpaqueData()
	{
	}

	OpaqueData::~OpaqueData()
	{
		for (auto& item : m_table)
		{

		}

		m_table.clear();
	}

	void OpaqueData::addProperties(const_iterator begin, const_iterator end)
	{
		for (auto it = begin; it != end; ++it)
		{
			uint8_t* bytes = it->second.m_bytes;
			uint32_t sizeInBytes = it->second.m_sizeInBytes;

			uint8_t* copyBytes = new uint8_t[sizeInBytes];

			memcpy(copyBytes, bytes, sizeInBytes);

			Data data(sizeInBytes, copyBytes);
			m_table[it->first] = data;
		}
	}
}