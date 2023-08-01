#include "TextureProcessor.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(TextureProcessor);

	TextureProcessor::TextureProcessor()
	{
		m_supportedExtensions = { L".dds", L".bmp", L".jpg" };
	}

	std::shared_ptr<ContentItem> TextureProcessor::Process(const std::wstring& assetFilename)
	{
		return nullptr;
	}
}