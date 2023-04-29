#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

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

		virtual ~Sink() = default;
		virtual void Commit(const std::string& outputMessage) = 0;

		friend class Logger;
	};

	class ConsoleSink : public Sink
	{
	public:
		ConsoleSink();
		ConsoleSink(std::ostream& stream);
		virtual ~ConsoleSink() = default;

		void Commit(const std::string& outputMessage) override;

	private:
		std::ostream& m_stream;
	};

	class FileSink : public Sink
	{
	public:
		FileSink(const std::string& filename);
		virtual ~FileSink();

		void Commit(const std::string& outputMessage) override;
	private:
		std::ofstream m_stream;
	};

	class Logger
	{
	public:
		explicit Logger() :
			m_sinks()
		{
		}

		template<typename It>
		Logger(It begin, It end) :
			m_sinks(begin, end)
		{
		}
		
		Logger(std::shared_ptr<Sink> sink) :
			Logger({ std::move(sink) })
		{
		}

		Logger(std::initializer_list<std::shared_ptr<Sink>> sinks) :
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

			char timeBuffer[32];
			std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			
			struct tm timeinfo;
			localtime_s(&timeinfo, &currentTime);

			strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H-%M-%S", &timeinfo);

			std::string logContent = m_stream.str();
			std::ostringstream builder;
			builder << "[" << timeBuffer << "] [" << GetLogLevelName(m_logLevel) << "] " << logContent;
			
			std::string ouputMessage = builder.str();

			for (auto sink : m_sinks)
			{
				//sink << ouputMessage;
				//sink.Flush();
				sink->Commit(ouputMessage);
			}

			m_stream.str({});
			m_stream.clear();
		}

	private:
		std::stringstream m_stream;
		LogLevel m_logLevel{ LogLevel::Trace };

		std::vector<std::shared_ptr<Sink>> m_sinks;

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
