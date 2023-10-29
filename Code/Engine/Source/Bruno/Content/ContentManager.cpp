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
		std::filesystem::path cleanPath(m_rootDirectory);
		cleanPath /= assetName + L".bruno";

		FileStream fileStream(cleanPath, FileAccess::Read);
		ContentReader contentReader(this, fileStream, assetName);
		return contentReader.ReadAsset();
	}
}