#include "ProcessorManager.h"

#include "Processors/TextureProcessor.h"
#include "Processors/ShaderProcessor.h"

namespace Bruno
{
	std::vector<std::shared_ptr<AbstractProcessor>> ProcessorManager::g_processors;
	bool ProcessorManager::g_initialized = false;

	const std::vector<std::shared_ptr<AbstractProcessor>>& ProcessorManager::GetProcessors()
	{
		return g_processors;
	}

	bool ProcessorManager::AddProcessor(std::shared_ptr<AbstractProcessor> processor)
	{
		g_processors.emplace_back(processor);
		return true;
	}

	const std::shared_ptr<AbstractProcessor>& ProcessorManager::GetProcessorByExtension(const std::wstring& extension)
	{
		for (auto& processor : g_processors)
		{
			auto& extensions = processor->GetExtensionsSupported();
			auto it = std::find(extensions.begin(), extensions.end(), extension);
			if (it != extensions.end())
				return processor;
		}
		return nullptr;
	}

	const std::shared_ptr<AbstractProcessor>& ProcessorManager::GetProcessorByName(const std::string& name)
	{
		for (auto& processor : g_processors)
		{
			if (processor->ToString() == name)
				return processor;
		}
		return nullptr;
	}

	void ProcessorManager::Initialize()
	{
		if (g_initialized)
			return;

		AddProcessor(std::make_shared<TextureProcessor>());
		AddProcessor(std::make_shared<ShaderProcessor>());

		g_initialized = true;
	}

	void ProcessorManager::Shutdown()
	{
		if (!g_initialized)
			return;

		g_initialized = false;
	}
}