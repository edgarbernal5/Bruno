#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>
#include <vector>
#include <filesystem>

namespace TrioEngine
{
	class BuildRequest;

	class TRIO_API_EXPORT BuildItem
	{
	public:
		BuildItem() : 
			m_IsBuilt(false), m_IsWanted(false), m_BuildRequest(nullptr)
		{

		}

		friend class BuildCoordinator;
		friend class BuildItemCollection;
		friend class BasicContentProcessorContext;
	private:
		BuildRequest* m_BuildRequest;

		std::map<std::string, std::chrono::system_clock::time_point> m_Dependencies;
		std::vector<std::string> m_ExtraOutputFiles;
		bool m_IsBuilt;
		bool m_IsWanted;
		std::string OutputFilename;
		std::filesystem::file_time_type m_SourceTimestamp;

		std::vector<std::string> m_Requests;
	};
}