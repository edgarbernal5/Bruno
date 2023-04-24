#pragma once

#include "Logger.h"

#define BR_CORE_INFO *Bruno::Log::GetCoreLogger()

namespace Bruno
{
	class Log
	{
	public:
		static void Initialize();
		static void Shutdown();

		static std::shared_ptr<Logger>& GetCoreLogger()
		{
			return g_CoreLogger;
		}
	private:
		static std::shared_ptr<Logger> g_CoreLogger;
	};
}
