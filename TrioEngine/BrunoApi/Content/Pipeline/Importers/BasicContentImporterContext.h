#pragma once

#include "TrioApiRequisites.h"

#include "ContentImporterContext.h"

namespace TrioEngine
{
	class BuildItem;
	class BuildCoordinator;
	class BuildRequest;

	class BRUNO_API_EXPORT BasicContentImporterContext : public ContentImporterContext
	{
	public:
		BasicContentImporterContext(BuildCoordinator *buildCoordinator, BuildItem *buildItem);

		void AddDependency(std::string filename);
		std::string GetIntermediateDirectory();
		std::string GetOutputDirectory();
	private:
		BuildCoordinator * m_buildCoordinator;
		BuildItem * m_buildItem;
	};
}