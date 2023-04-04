using System;

namespace Bruno.Logging
{
    public class DebugLog : LogBase
    {
        public override void Debug(string message)
        {
            System.Diagnostics.Debug.WriteLine(GetLogMessage(message, LogType.DEBUG));
        }

        public override void Debug(string format, params object[] args)
        {
            System.Diagnostics.Debug.WriteLine(GetLogMessage(format, LogType.DEBUG, args));
        }

        public override void Info(string format, params object[] args)
        {
            System.Diagnostics.Debug.WriteLine(GetLogMessage(format, LogType.INFO, args));
        }

        public override void Error(Exception exception)
        {
            System.Diagnostics.Debug.WriteLine(GetLogMessage(exception.ToString(), LogType.ERROR));
        }

        public override void Warn(string format, params object[] args)
        {
            System.Diagnostics.Debug.WriteLine(GetLogMessage(format, LogType.WARN, args));
        }
    }
}