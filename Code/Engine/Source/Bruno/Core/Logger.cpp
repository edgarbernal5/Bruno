#include "brpch.h"
#include "Logger.h"

namespace Bruno
{
	Sink::Sink() :
		m_stream(std::cout)
	{

	}
	Sink::Sink(std::ostream& stream) :
		m_stream(stream)
	{
	}

	Logger::Logger(const Logger& other) :
		m_sinks(other.m_sinks),
		m_logLevel(other.m_logLevel)
	{
	}
}