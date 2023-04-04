#pragma once

#include "TrioApiRequisites.h"

#include "ContentImporterContext.h"

namespace BrunoEngine
{
	class BuildItem;
	class GameContentBuilder;
	class BuildRequest;

	class BRUNO_API_EXPORT BasicContentImporterContext : public ContentImporterContext
	{
	public:
		BasicContentImporterContext(GameContentBuilder *buildCoordinator, BuildItem *buildItem);

		void AddDependency(std::string filename);
		std::string GetIntermediateDirectory();
		std::string GetOutputDirectory();
	private:
		GameContentBuilder * m_buildCoordinator;
		BuildItem * m_buildItem;
	};
}