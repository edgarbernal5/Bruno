#pragma once

#include <map>
#include <Bruno/Core/RTTI.h>
#include "AbstractProcessor.h"

namespace Bruno
{
	class ProcessorManager
	{
	public:
		ProcessorManager() = delete;
		~ProcessorManager() = default;

		static const std::map<RTTI::IdType, std::shared_ptr<AbstractProcessor>>& GetProcessors();
		static bool AddProcessor(std::shared_ptr<AbstractProcessor> processor);

		static void Initialize();
		static void Shutdown();

	private:
		static std::map<RTTI::IdType, std::shared_ptr<AbstractProcessor>> g_processors;
		static bool g_initialized;
	};
}

