#pragma once

#include <string>
#include <vector>

namespace BrunoIO
{
	class Stream;

	class IStorage
	{
	public:
		virtual ~IStorage(){}

		virtual std::string GetRealPath(std::string path) const = 0;
		//virtual std::vector<std::string> GetFiles(std::string path) const = 0;
		//virtual std::vector<std::string> GetDirectories(std::string path) const = 0;

		virtual Stream* OpenFile(std::string path) = 0;
	};
}