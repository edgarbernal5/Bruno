#include "ShaderProcessor.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderProcessor);

	Bruno::ShaderProcessor::ShaderProcessor()
	{
		m_supportedExtensions = { L".hlsl", L".fx" };
	}

	ContentItem ShaderProcessor::Process(const std::wstring& assetFilename)
	{
		return ContentItem();
	}
}