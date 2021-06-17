#include "stdafx.h"
#include "ProcessorManager.h"

#include "IContentProcessor.h"

#include "Content/Pipeline/Processors/TextureProcessor.h"
#include "Content/Pipeline/Processors/ModelProcessor.h"
//#include "Content/Pipeline/Processors/SceneModelProcessor.h"
#include "Content/Pipeline/Processors/MaterialProcessor.h"
#include "Content/Pipeline/Processors/EffectProcessor.h"

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
		RegisterTypeProcessor<TextureProcessor>("TextureProcessor");
		//RegisterTypeProcessor<SceneModelProcessor>("SceneModelProcessor");
		RegisterTypeProcessor<ModelProcessor>("ModelProcessor");
		RegisterTypeProcessor<MaterialProcessor>("MaterialProcessor");
		RegisterTypeProcessor<EffectProcessor>("EffectProcessor");
	}

	IContentProcessor* ProcessorManager::GetByProcessorName(std::string const& readerName, OpaqueData* opaqueData)
	{
		map_type::iterator it = m_importersMap.find(readerName);
		if (it != m_importersMap.end())
		{
			//TO-DO: pasar el OpaqueData.
			return it->second();
		}

		return nullptr;
	}
}