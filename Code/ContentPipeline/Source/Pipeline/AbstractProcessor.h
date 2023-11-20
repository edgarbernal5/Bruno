#pragma once

#include "Bruno/Core/RTTI.h"
#include "Pipeline/ContentItem.h"
#include "Pipeline/ContentProcessorContext.h"
#include <string>
#include <vector>

namespace Bruno
{
	class AbstractProcessor : public RTTI
	{
		BR_RTTI_DECLARATION(AbstractProcessor, RTTI);

	public:
		virtual ~AbstractProcessor() = default;

		virtual std::shared_ptr<ContentItem> Process(const std::wstring& assetFilename, ContentProcessorContext& context) = 0;

		const std::vector<std::wstring>& GetExtensionsSupported() { return m_supportedExtensions; }

	protected:
		std::vector<std::wstring> m_supportedExtensions;

	private:

	};
}