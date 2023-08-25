#include "brpch.h"
#include "ContentManager.h"

#include "ContentTypeReaderManager.h"
#include "ContentReader.h"
#include "Bruno/Core/FileStream.h"

namespace Bruno
{
	const std::wstring ContentManager::DefaultRootDirectory{ L"Content\\" };

	ContentManager::ContentManager(const std::wstring& rootDirectory) :
		m_rootDirectory(rootDirectory)
	{
	}

	std::shared_ptr<RTTI> ContentManager::ReadAsset(const std::wstring& assetName)
	{
		FileStream fileStream(assetName + L".bruno", FileAccess::Read);
		ContentReader contentReader(this, fileStream, assetName);
		return contentReader.ReadAsset();
	}
}