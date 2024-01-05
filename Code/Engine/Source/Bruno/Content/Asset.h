#pragma once

#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	using AssetHandle = uint64_t;

	class Asset : public RTTI
	{
		BR_RTTI_DECLARATION(Asset, RTTI);

	public:
		virtual ~Asset() {}

		virtual bool operator==(const Asset& other) const
		{
			return m_assetHandle == other.m_assetHandle;
		}

		virtual bool operator!=(const Asset& other) const
		{
			return !(*this == other);
		}

	private:
		AssetHandle m_assetHandle{ 0 };
	};
}