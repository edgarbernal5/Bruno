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
			return m_handle == other.m_handle;
		}

		bool operator!=(const Asset& other) const
		{
			return !(*this == other);
		}

		AssetHandle GetHandle() const { return m_handle; }

	protected:
		AssetHandle m_handle{ 0 };
	};
}