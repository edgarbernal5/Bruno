#pragma once

#include "ContentWriter.h"
#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	class AbstractContentTypeWriter : public RTTI
	{
		BR_RTTI_DECLARATION(AbstractContentTypeWriter, RTTI);
	public:
		virtual ~AbstractContentTypeWriter() = default;

		virtual std::string GetRuntimeReader();
		RTTI::IdType GetTargetTypeId() const;
		virtual void Write(ContentWriter& output, const ContentItem& contentItem) = 0;

	protected:
		AbstractContentTypeWriter(const RTTI::IdType targetTypeId);

		const RTTI::IdType m_targetTypeId;
	};
}

