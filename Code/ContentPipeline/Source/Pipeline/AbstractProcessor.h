#pragma once

#include <Bruno/Core/RTTI.h>
#include "Pipeline/ContentItem.h"
#include <string>
#include <vector>

namespace Bruno
{
	class AbstractProcessor : public RTTI
	{
		BR_RTTI_DECLARATION(AbstractProcessor, RTTI);
	public:
		virtual ~AbstractProcessor() = default;

		virtual ContentItem* Process(const std::wstring& assetFilename) = 0;

		const std::vector<std::wstring>& GetExtensionsSupported()
		{
			return m_supportedExtensions;
		}

	protected:
		std::vector<std::wstring> m_supportedExtensions;

	private:

	};
}