#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>
#include "Content/Pipeline/OpaqueData.h"

namespace TrioEngine
{
	class IContentProcessor;

	class TRIO_API_EXPORT ProcessorManager
	{
	public:
		ProcessorManager();
		~ProcessorManager();

		IContentProcessor* GetByProcessorName(std::string const& readerName, OpaqueData* opaqueData);

		void RegisterStandardTypes();

		template<typename T>
		void RegisterTypeProcessor(std::string importerName);
	private:

		template<typename T>
		static IContentProcessor* CreateInstance();

		typedef std::map<std::string, IContentProcessor*(*)()> map_type;

		map_type importersMap;
	};

	template<typename T>
	IContentProcessor* ProcessorManager::CreateInstance() { return new T(); }

	template<typename T>
	void ProcessorManager::RegisterTypeProcessor(std::string importerName)
	{
		importersMap[importerName] = &CreateInstance <T>;
	}
}