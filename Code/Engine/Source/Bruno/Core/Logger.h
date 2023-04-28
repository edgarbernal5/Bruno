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
		Trace = 0,
		Debug,
		Info,
		Warn,
		Error,
		Fatal
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

		virtual void Flush()
		{
			//m_stream.flush();
			m_stream.clear();
		}

		friend class Logger;
	protected:
		std::ostream& m_stream;
		//inline virtual std::ostream& Stream() = 0;
	};

	/*class ConsoleSink : public Sink
	{
	public:
		ConsoleSink();
		virtual ~ConsoleSink() = default;
	private:
		std::ostream& m_stream;

	protected:
		inline std::ostream& Stream() override;
	};

	/*class FileSink : public Sink
	{
	public:
		FileSink();
		FileSink(std::string filename);
		virtual ~FileSink() = default;

	private:
		std::ofstream& m_stream;

	protected:
		inline std::ostream& Stream() override;
	};*/

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
			m_stream << outputMessage;
			return *this;
		}

		Logger& operator<<(ManipFn manip) /// endl, flush, setw, setfill, etc.
		{
			manip(m_stream);

			if (manip == static_cast<ManipFn>(std::flush)
				|| manip == static_cast<ManipFn>(std::endl))
				Flush();

			return *this;
		}

		Logger& operator<<(FlagsFn manip) /// setiosflags, resetiosflags
		{
			manip(m_stream);

			return *this;
		}

		Logger& operator()(LogLevel level)
		{
			m_logLevel = level;
			return *this;
		}

		void Flush()
		{
			/*
			  m_stream.str() has your full message here.
			  Good place to prepend time, log-level.
			  Send to console, file, socket, or whatever you like here.
			*/
			//https://github.com/i42output/neolib/blob/master/include/neolib/app/i_logger.hpp

			std::string logContent = m_stream.str();
			for (auto& sink : m_sinks)
			{
				sink << GetLogLevelName(m_logLevel) << " " << logContent;
				sink.Flush();
			}

			m_stream.str({});
			m_stream.clear();
		}

	private:
		std::stringstream m_stream;
		LogLevel m_logLevel{ LogLevel::Trace };

		std::vector<Sink> m_sinks;

		const char* GetLogLevelName(LogLevel level)
		{
			if (level == LogLevel::Trace) return "Trace";
			if (level == LogLevel::Debug) return "Debug";
			if (level == LogLevel::Info) return "Info";
			if (level == LogLevel::Warn) return "Warn";
			if (level == LogLevel::Error) return "Error";
			if (level == LogLevel::Fatal) return "Fatal";
			return "";
		}
	};
}
