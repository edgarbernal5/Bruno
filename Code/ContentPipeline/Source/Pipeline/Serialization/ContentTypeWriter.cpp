#include "ContentTypeWriter.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(AbstractContentTypeWriter);

	AbstractContentTypeWriter::AbstractContentTypeWriter(const RTTI::IdType targetTypeId) :
		m_targetTypeId(targetTypeId)
	{
	}

	//std::string AbstractContentTypeWriter::GetReaderName()
	//{
	//	auto writerName = this->ToString();
	//	if (!writerName.empty() && writerName.find_last_of("Writer") == writerName.size() - 1)
	//	{
	//		writerName = writerName.substr(0, writerName.size() - 6) + "Reader";
	//	}
	//	return writerName;
	//}

	RTTI::IdType AbstractContentTypeWriter::GetTargetTypeId() const
	{
		return m_targetTypeId;
	}
}