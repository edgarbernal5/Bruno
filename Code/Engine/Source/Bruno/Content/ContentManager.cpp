#include "brpch.h"
#include "ContentManager.h"

#include "ContentTypeReaderManager.h"

namespace Bruno
{
	const std::string ContentManager::DefaultRootDirectory{ "Content\\" };

	ContentManager::ContentManager(const std::string& rootDirectory) :
		m_rootDirectory(rootDirectory)
	{
	}
	std::shared_ptr<RTTI> ContentManager::ReadAsset(const RTTI::IdType targetTypeId, const std::string& assetName)
	{
		const auto& contentTypeReaders = ContentTypeReaderManager::GetContentTypeReaders();
		auto it = contentTypeReaders.find(targetTypeId);
		if (it == contentTypeReaders.end())
		{
			throw std::exception("Content type reader not registered.");
		}

		auto& reader = it->second;
		return reader->Read(assetName);
	}
}