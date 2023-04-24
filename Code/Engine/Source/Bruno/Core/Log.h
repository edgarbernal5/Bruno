#pragma once

#include "Logger.h"

#define BR_CORE_INFO Bruno::Log::GetCoreLogger()

namespace Bruno
{
	class Log
	{
	public:
		static Logger& GetCoreLogger()
		{
			return g_CoreLogger;
		}
	private:
		static Logger g_CoreLogger;
	};
}
