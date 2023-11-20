#pragma once

#include <vector>
#include "Bruno/Core/RTTI.h"
#include "AbstractProcessor.h"

namespace Bruno
{
	class ProcessorManager
	{
	public:
		ProcessorManager() = delete;
		~ProcessorManager() = default;

		static const std::vector<std::shared_ptr<AbstractProcessor>>& GetProcessors();
		static bool AddProcessor(std::shared_ptr<AbstractProcessor> processor);

		static std::shared_ptr<AbstractProcessor> GetProcessorByExtension(const std::wstring& extension);
		static std::shared_ptr<AbstractProcessor> GetProcessorByName(const std::string& name);
		static void Initialize();
		static void Shutdown();

	private:
		static std::vector<std::shared_ptr<AbstractProcessor>> g_processors;
		static bool g_initialized;
	};
}

