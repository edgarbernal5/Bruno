#include "ExternalReferenceContentItem.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ExternalReferenceContentItem);

	ExternalReferenceContentItem::ExternalReferenceContentItem(const std::wstring& filename) :
		Filename(filename)
	{
	}
}
