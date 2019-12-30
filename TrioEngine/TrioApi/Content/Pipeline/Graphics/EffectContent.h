#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT EffectContent : public ContentItem
	{
	public:
		EffectContent();
		EffectContent(std::string& effectCode);
		~EffectContent();

		inline std::string& GetEffectCode()
		{
			return m_effectCode;
		}

	private:
		std::string m_effectCode;
	};
}