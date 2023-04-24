#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace Bruno
{
	//https://github.com/i42output/neolib/blob/master/include/neolib/app/i_logger.hpp

	//https://stackoverflow.com/questions/40273809/how-to-write-iostream-like-interface-to-logging-library

	//https://www.cppstories.com/2021/stream-logger/

	/*class Sink
	{
	public:
		virtual ~Sink() = default;

		Sink& operator<<(const std::string& message);
	private:

	};*/

	class Logger
	{
	public:
		typedef std::ostream& (*ManipFn)(std::ostream&);
		typedef std::ios_base& (*FlagsFn)(std::ios_base&);
		//typedef std::ostringstream Buffer;

		Logger();
		~Logger() = default;
		
		enum class LogLevel
		{
			DEBUG,
			INFO,
			WARN,
			//ERROR
		};

		template<class T>
		Logger& operator<<(const T& outputMessage)
		{
			m_stream << outputMessage;
			return *this;
		}

		Logger& operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
		{
			manip(m_stream);

			if (manip == static_cast<ManipFn>(std::flush)
				|| manip == static_cast<ManipFn>(std::endl))
				this->Flush();

			return *this;
		}

		Logger& operator<<(FlagsFn manip) /// setiosflags, resetiosflags
		{
			manip(m_stream);
			return *this;
		}

		Logger& operator()(LogLevel e)
		{
			m_logLevel = e;
			return *this;
		}

		void Flush()
		{
			/*
			  m_stream.str() has your full message here.
			  Good place to prepend time, log-level.
			  Send to console, file, socket, or whatever you like here.
			*/

			m_logLevel = LogLevel::INFO;

			//m_stream.str(std::string());
			m_stream.clear();
		}

		//template<typename... Args>
		//Logger& operator<<(Args&&... args);
	private:
		//std::vector<Sink> m_sinks;

		std::ostream &m_stream;
		LogLevel m_logLevel{ LogLevel::DEBUG };
	};
	
	/*template<typename ...Args>
	inline Logger& Logger::operator<<(Args && ...args)
	{
		return *this;
	}*/
}
