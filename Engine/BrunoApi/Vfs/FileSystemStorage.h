#pragma once

#include "Vfs/IStorage.h"
#include <string>
namespace BrunoIO
{
	class FileSystemStorage : public IStorage
	{
	public:
		FileSystemStorage(std::string rootDirectory);
		~FileSystemStorage();

		std::string GetRealPath(std::string path) const override;

		Stream* OpenFile(std::string path) override;
		
	private:
		std::string m_rootDirectory;
	};
}