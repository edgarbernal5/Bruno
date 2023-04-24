#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace Bruno
{
	//https://github.com/i42output/neolib/blob/master/include/neolib/app/i_logger.hpp

	//https://stackoverflow.com/questions/40273809/how-to-write-iostream-like-interface-to-logging-library

	//https://www.cppstories.com/2021/stream-logger/
	
	enum class LogLevel : uint8_t
	{
		Debug = 0,
		Info,
		Warn,
		Error,
	};

	typedef std::ostream& (*ManipFn)(std::ostream&);
	typedef std::ios_base& (*FlagsFn)(std::ios_base&);

	class Logger;

	class Sink
	{
	public:
		//typedef std::ostringstream Buffer;

		Sink();
		Sink(std::ostream& stream);
		~Sink() = default;


		template<class T>
		Sink& operator<<(const T& outputMessage)
		{
			m_stream << outputMessage;
			return *this;
		}

		Sink& operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
		{
			manip(m_stream);

			if (manip == static_cast<ManipFn>(std::flush)
				|| manip == static_cast<ManipFn>(std::endl))
				this->Flush();

			return *this;
		}

		Sink& operator<<(FlagsFn manip) /// setiosflags, resetiosflags
		{
			manip(m_stream);
			return *this;
		}

		void Flush()
		{
			/*
			  m_stream.str() has your full message here.
			  Good place to prepend time, log-level.
			  Send to console, file, socket, or whatever you like here.
			*/

			//m_stream.str(std::string());
			m_stream.clear();
		}
		
		friend class Logger;
	protected:
		std::ostream& m_stream;
	};

	class Logger
	{
	public:
		Logger() = default;

		template<typename It>
		Logger(It begin, It end) :
			m_sinks(begin, end)
		{

		}

		Logger(std::initializer_list<Sink> sinks) :
			m_sinks(sinks.begin(), sinks.end())
		{

		}
		
		~Logger() = default;

		Logger(const Logger& other);

		template<class T>
		Logger& operator<<(const T& outputMessage)
		{
			for (auto& sink : m_sinks)
			{
				sink << outputMessage;
			}
			return *this;
		}

		Logger& operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
		{
			for (auto& sink : m_sinks)
			{
				manip(sink.m_stream);

				if (manip == static_cast<ManipFn>(std::flush)
					|| manip == static_cast<ManipFn>(std::endl))
				{
					sink.Flush();
				}
			}
			return *this;
		}

		Logger& operator<<(FlagsFn manip) /// setiosflags, resetiosflags
		{
			for (auto& sink : m_sinks)
			{
				manip(sink.m_stream);
			}
			return *this;
		}

		Logger& operator()(LogLevel level)
		{
			m_logLevel = level;
			return *this;
		}

		void Flush()
		{
			for (auto& sink : m_sinks)
			{
				sink.Flush();
			}
		}

	protected:
		std::vector<Sink> m_sinks;
		LogLevel m_logLevel{ LogLevel::Debug };
	};
}
