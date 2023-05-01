#pragma once

#include "Logger.h"

#define BR_CORE_TRACE (*Bruno::Log::GetCoreLogger())(LogLevel::Trace)
#define BR_CORE_DEBUG (*Bruno::Log::GetCoreLogger())(LogLevel::Debug)

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

		static void PrintAssertMessage(const char* message, const char* file, int line);
	private:
		static std::shared_ptr<Logger> g_CoreLogger;
	};
}
