#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>
#include <vector>
#include <filesystem>

namespace BrunoEngine
{
	class BuildRequest;

	class BRUNO_API_EXPORT BuildItem
	{
	public:
		BuildItem();

		bool ContainsDependency(std::string filename);
		bool ContainsRequest(std::string filename);

		void RemoveDependency(std::string filename);
		void RemoveRequest(std::string filename);

		friend class BuildCoordinator;
		friend class BuildItemCollection;
		friend class BasicContentProcessorContext;
	private:
		BuildRequest* m_buildRequest;

		std::map<std::string, std::filesystem::file_time_type> m_dependencies;
		std::vector<std::string> m_extraOutputFiles;
		bool m_isBuilt;
		bool m_isWanted;
		std::string m_outputFilename;
		std::filesystem::file_time_type m_sourceTimestamp;

		std::vector<std::string> m_requests;
	};
}