#include "stdafx.h"
#include "FileSystemStorage.h"

#include "IO/FileStream.h"
#include "IO/Path.h"

namespace BrunoIO
{
	FileSystemStorage::FileSystemStorage(std::string rootDirectory) :
		m_rootDirectory(rootDirectory)
	{
		if (m_rootDirectory.size() == 0) m_rootDirectory = ".";

		m_rootDirectory = BrunoIO::Path::GetFullDirectory(m_rootDirectory + BrunoIO::Path::DirectorySeparator);

		//m_rootDirectory = BrunoIO::Path::Normalize(m_rootDirectory);
	}

	FileSystemStorage::~FileSystemStorage()
	{
	}

	std::string FileSystemStorage::GetRealPath(std::string path) const
	{
		return BrunoIO::Path::Combine(m_rootDirectory, path);
	}

	Stream* FileSystemStorage::OpenFile(std::string path)
	{
		std::string realPath = BrunoIO::Path::Combine(m_rootDirectory, path);
		return new BrunoIO::FileStream(realPath, BrunoIO::FileAccess::Read);
	}

}