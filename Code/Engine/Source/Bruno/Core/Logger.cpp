#include "brpch.h"
#include "Logger.h"

#include <fstream>

namespace Bruno
{
	Sink::Sink() :
		m_stream(std::cout)
	{
	}

	Sink::Sink(std::ostream& stream) :
		m_stream{ stream }
	{
	}

	Logger::Logger(const Logger& other) :
		m_sinks(other.m_sinks),
		m_logLevel(other.m_logLevel)
	{
	}
	/*
	FileSink::FileSink() :
		//Sink(),
		m_stream(std::cout)
	{
	}

	FileSink::FileSink(std::string filename) //: 
		//Sink(),
		//m_stream(std::cout)
	{
		std::ofstream outfile;
		m_stream = outfile;
	}

	std::ostream& FileSink::Stream()
	{
		return m_stream;
	}*/

	/*ConsoleSink::ConsoleSink() :
		//Sink(),
		m_stream(std::cout)
	{
	}*/

}