#include "stdafx.h"
#include "ProcessorManager.h"

#include "IContentProcessor.h"

namespace TrioEngine
{
	ProcessorManager::ProcessorManager()
	{
		RegisterStandardTypes();
	}

	ProcessorManager::~ProcessorManager()
	{
	}

	void ProcessorManager::RegisterStandardTypes()
	{
		//RegisterTypeProcessor<TextureProcessor>("TextureProcessor");
		//RegisterTypeProcessor<ModelProcessor>("ModelProcessor");
		////RegisterTypeProcessor<MaterialProcessor>("MaterialProcessor");
		//RegisterTypeProcessor<EffectProcessor>("EffectProcessor");
	}

	IContentProcessor* ProcessorManager::GetByProcessorName(std::string const& readerName, OpaqueData* opaqueData)
	{
		map_type::iterator it = importersMap.find(readerName);
		if (it != importersMap.end())
		{
			//TO-DO: pasar el OpaqueData.
			return it->second();
		}

		return nullptr;
	}
}