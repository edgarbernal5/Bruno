#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT CompiledEffectContent : public ContentItem
	{
	public:
		CompiledEffectContent();
		CompiledEffectContent(std::vector<uint8_t>& bytes);
		~CompiledEffectContent();

		inline std::vector<uint8_t>& GetEffectCode()
		{
			return m_bytes;
		}

	private:
		std::vector<uint8_t> m_bytes;
	};
}