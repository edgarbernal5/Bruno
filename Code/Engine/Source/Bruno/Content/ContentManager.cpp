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
	std::shared_ptr<RTTI> ContentManager::ReadAsset(const RTTI::IdType targetTypeId, const std::wstring& assetName)
	{
		const auto& contentTypeReaders = ContentTypeReaderManager::GetContentTypeReaders();
		auto it = contentTypeReaders.find(targetTypeId);
		if (it == contentTypeReaders.end())
		{
			throw std::exception("Content type reader not registered.");
		}

		FileStream fileStream(assetName, FileAccess::Read);
		ContentReader contentReader(this, fileStream, assetName);
		return contentReader.ReadAsset();

		//auto& reader = it->second;
		//return reader->Read(assetName);
	}
}