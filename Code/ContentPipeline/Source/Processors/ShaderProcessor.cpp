#include "ShaderProcessor.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ShaderProcessor);

	Bruno::ShaderProcessor::ShaderProcessor()
	{
		m_supportedExtensions = { L".hlsl", L".fx" };
	}

	void ShaderProcessor::Process(const std::wstring& assetFilename)
	{

	}
}