#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/OpaqueData.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT BuildRequest
	{
	public:
		BuildRequest();
		BuildRequest(const BuildRequest& other);
		BuildRequest(const BuildRequest* & other);
		~BuildRequest();

		bool operator <(const BuildRequest &other) const
		{
			return m_sourceFileName < other.m_sourceFileName;
		}
		bool operator <(const BuildRequest * other) const
		{
			return m_sourceFileName < other->m_sourceFileName;
		}
		bool operator <(const BuildRequest* &other) const
		{
			return m_sourceFileName < other->m_sourceFileName;
		}

		inline std::string& GetSourceFilename() {
			return m_sourceFileName;
		}

		BuildRequest& operator= (const BuildRequest&) = default;

		friend class BuildCoordinator;
		friend class BuildItemCollection;
		friend class BasicContentProcessorContext;

		friend class Compare_P;
	private:
		std::string m_assetName;
		std::string m_sourceFileName;
		std::string m_importerName;
		std::string m_processorName;
		OpaqueData* m_opaqueData;
	};

	//bool operator<(const BuildRequest& a, const BuildRequest& b);

	/*bool operator<(const BuildRequest& a, const BuildRequest& b) {
		return a.GetSourceFilename() < b.GetSourceFilename();
	}*/
}