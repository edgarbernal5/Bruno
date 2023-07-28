#include "ProcessorManager.h"

namespace Bruno
{
	std::map<RTTI::IdType, std::shared_ptr<AbstractProcessor>> ProcessorManager::g_processors;
	bool ProcessorManager::g_initialized = false;

	const std::map<RTTI::IdType, std::shared_ptr<AbstractProcessor>>& ProcessorManager::GetProcessors()
	{
		return g_processors;
	}

	bool ProcessorManager::AddProcessor(std::shared_ptr<AbstractProcessor> processor)
	{
		return false;
	}

	void ProcessorManager::Initialize()
	{
		if (g_initialized)
			return;

		g_initialized = true;
	}

	void ProcessorManager::Shutdown()
	{
		if (!g_initialized)
			return;

		g_initialized = false;
	}
}