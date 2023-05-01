#include "brpch.h"
#include "Log.h"

namespace Bruno
{
	std::shared_ptr<Logger> Log::g_CoreLogger;

	void Log::Initialize()
	{
		std::vector<std::shared_ptr<Sink>> coreSinks = {
			std::make_shared<ConsoleSink>(),
			std::make_shared<FileSink>("LogTest.log"),
		};

		g_CoreLogger = std::make_shared<Logger>(coreSinks.begin(), coreSinks.end());
	}

	void Log::Shutdown()
	{
		g_CoreLogger.reset();
	}

	void Log::PrintAssertMessage(const char* message, const char* file, int line)
	{
		std::ostringstream builder;
		builder << message << " at file " << file << ". Line " << line;
		*(g_CoreLogger) << builder.str() << std::endl;
	}
}