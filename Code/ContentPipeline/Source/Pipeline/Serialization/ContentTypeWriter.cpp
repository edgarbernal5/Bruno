#include "ContentTypeWriter.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(AbstractContentTypeWriter);

	AbstractContentTypeWriter::AbstractContentTypeWriter(const RTTI::IdType targetTypeId) :
		m_targetTypeId(targetTypeId)
	{
	}

	RTTI::IdType AbstractContentTypeWriter::GetTargetTypeId() const
	{
		return m_targetTypeId;
	}
}