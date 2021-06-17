#include "stdafx.h"
#include "ContentPipelineException.h"

namespace TrioEngine
{
	ContentPipelineException::ContentPipelineException(const char* const& message)
		: exception(message)
	{
	}

	std::wstring ContentPipelineException::whatw() const
	{
		std::string whatString(what());
		std::wstring whatw;
		whatw.assign(whatString.begin(), whatString.end());

		return whatw;
	}
}