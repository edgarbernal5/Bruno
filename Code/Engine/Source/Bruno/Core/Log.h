#pragma once

#include "Logger.h"

#define BR_CORE_TRACE (*Bruno::Log::GetCoreLogger())(LogLevel::Trace)
#define BR_CORE_DEBUG (*Bruno::Log::GetCoreLogger())(LogLevel::Debug)
#define BR_CORE_WARN (*Bruno::Log::GetCoreLogger())(LogLevel::Warn)
#define BR_CORE_ERROR (*Bruno::Log::GetCoreLogger())(LogLevel::Error)

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

		template<typename... Args>
		static void PrintAssertMessage(const char* file, int line, const char* message, Args&&... args);
	private:
		static std::shared_ptr<Logger> g_CoreLogger;
	};


	template<typename... Args>
	void Log::PrintAssertMessage(const char* file, int line, const char* message, Args&&... args)
	{
		//https://stackoverflow.com/questions/27375089/what-is-the-easiest-way-to-print-a-variadic-parameter-pack-using-stdostream
		//https://learn.microsoft.com/en-us/cpp/cpp/ellipses-and-variadic-templates?view=msvc-170

		std::ostringstream builder;
		builder << message;
		
		((builder << ". " << std::forward<Args>(args)), ...);
		builder << std::endl << "File: " << file << ". Line: " << line;
		
		(*g_CoreLogger)(LogLevel::Error) << builder.str() << std::endl;
	}
}
