
using System;

namespace Bruno.Logging
{
    public abstract class LogBase : ILog
    {
        private const string DateTimeFormat = "yyyy-MM-dd HH:mm:ss.FFF";
        public abstract void Debug(string message);

        public abstract void Debug(string format, params object[] args);

        public abstract void Error(Exception exception);

        public abstract void Info(string format, params object[] args);

        public abstract void Warn(string format, params object[] args);

        protected virtual string GetLogMessage(string format, LogType logType, params object[] args)
        {
            var className = Logger.GetClassFullName(4);
            return string.Format("[{0}] {1} {2}: {3}",
                DateTime.Now.ToString(DateTimeFormat), logType, className, string.Format(format, args));
        }

        protected virtual string GetLogMessage(string message, LogType logType)
        {
            var className = Logger.GetClassFullName(4);
            return string.Format("[{0}] {1} {2}: {3}",
                DateTime.Now.ToString(DateTimeFormat), logType, className, message);
        }
    }

    public enum LogType
    {
        DEBUG,
        ERROR,
        INFO,
        WARN
    }

    public enum LogLevel
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical,
        Off
    }
}
