#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/OpaqueData.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT BuildRequest
	{
	public:
		BuildRequest();
		~BuildRequest();

		bool operator <(const BuildRequest &other) const
		{
			return m_SourceFileName < other.m_SourceFileName;
		}
		bool operator <(const BuildRequest *other) const
		{
			return m_SourceFileName < other->m_SourceFileName;
		}

		friend class BuildCoordinator;
		friend class BasicContentProcessorContext;
	private:
		std::string m_AssetName;
		std::string m_SourceFileName;
		std::string m_ImporterName;
		std::string m_ProcessorName;
		OpaqueData* m_OpaqueData;
	};
}