#pragma once

#include "Bruno/Core/RTTI.h"
#include "Bruno/Core/UUID.h"

namespace Bruno
{
	using AssetHandle = UUID;

	class Asset : public RTTI
	{
		BR_RTTI_DECLARATION(Asset, RTTI);

	public:
		virtual ~Asset() {}

		bool operator==(const Asset& other) const
		{
			return m_assetHandle == other.m_assetHandle;
		}

		bool operator!=(const Asset& other) const
		{
			return !(*this == other);
		}

		AssetHandle Handle() const { return m_assetHandle; }

	protected:
		AssetHandle m_assetHandle{ 0 };
	};
}