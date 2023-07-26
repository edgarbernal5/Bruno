#include "brpch.h"
#include "ContentTypeReader.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(AbstractContentTypeReader);

	AbstractContentTypeReader::AbstractContentTypeReader(const RTTI::IdType targetTypeId) :
		m_targetTypeId(targetTypeId)
	{
	}

	RTTI::IdType AbstractContentTypeReader::GetTargetTypeId() const
	{
		return m_targetTypeId;
	}
}